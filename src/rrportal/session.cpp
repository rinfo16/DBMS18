#include <cstdlib>
#include "connection.h"
#include "session.h"
#include "assert.h"
//----------------------------------------------------------------------
using boost::asio::buffer;

Session::Session(tcp::socket socket, ConnectionManager& manager_)
    : socket_(std::move(socket)),
      manager_(manager_),
      proto_version_(0) {
}

void Session::start() {
  do {
    manager_.join(shared_from_this());
    if (!ProcessStartupPacket(false)) {
      break;
    }
    if (!SendParameterStatus()) {
      break;
    }
    SendBackendKeyData();
    SendReadForQuery(READY_FOR_QUERY_IDLE);
    while (1) {
      ReadCommand();
    }
  } while (0);
  manager_.leave(shared_from_this());
}

void Session::deliver(const MessageBuffer& msg) {
  bool write_in_progress = !write_msgs_.empty();
  write_msgs_.push_back(msg);
  if (!write_in_progress) {
    do_write();
  }
}

void Session::do_read_header() {
  auto self(shared_from_this());
  boost::asio::async_read(
      socket_,
      boost::asio::buffer(read_msg_.Data(), MessageBuffer::header_length),
      [this, self](boost::system::error_code ec, std::size_t /*length*/)
      {
        if (!ec && read_msg_.decode_header())
        {
          do_read_body();
        }
        else
        {
          manager_.leave(shared_from_this());
        }
      });
}

void Session::do_read_body() {
  auto self(shared_from_this());
  boost::asio::async_read(
      socket_, boost::asio::buffer(read_msg_.Data(), read_msg_.GetSize()),
      [this, self](boost::system::error_code ec, std::size_t /*length*/)
      {
        if (!ec)
        {
          manager_.deliver(read_msg_);
          do_read_header();
        }
        else
        {
          manager_.leave(shared_from_this());
        }
      });
}

void Session::do_write() {
  auto self(shared_from_this());
  boost::asio::async_write(
      socket_,
      boost::asio::buffer(write_msgs_.front().Data(),
                          write_msgs_.front().GetSize()),
      [this, self](boost::system::error_code ec, std::size_t /*length*/)
      {
        if (!ec)
        {
          write_msgs_.pop_front();
          if (!write_msgs_.empty())
          {
            do_write();
          }
        }
        else
        {
          manager_.leave(shared_from_this());
        }
      });
}

bool Session::ProcessStartupPacket(bool ssl_done) {
  uint32_t len;
  ProtocolVersion proto;
  auto self(shared_from_this());
  std::size_t size = boost::asio::read(socket_, boost::asio::buffer(&len, 4));
  if (size < 4) {
    return false;
  }
  len = ntohl(len);
  len -= 4;
  /*
   if (len <= sizeof(StartupPacket)) {
   len = sizeof(StartupPacket) + 1;
   } else {
   len = len + 1;
   }
   */
  read_msg_.ReAlloc(len);
  read_msg_.SetSize(len);
  size = boost::asio::read(
      socket_, boost::asio::buffer(read_msg_.Data(), read_msg_.GetSize()));
  if (size < len) {
    return false;
  }

  StartupPacket *packet = reinterpret_cast<StartupPacket*>(read_msg_.Data());
  packet->protoVersion = proto = ntohl(packet->protoVersion);

  if (proto == CANCEL_REQUEST_CODE) {
    /* Not really an error, but we don't want to proceed further */
    return false;
  }

  if (proto == NEGOTIATE_SSL_CODE && !ssl_done) {
    char SSLok;

#ifdef USE_SSL

#else
    SSLok = 'N'; /* No support for SSL */
#endif

    if (boost::asio::write(socket_, buffer(&SSLok, 1)) != 1) {
      return false; /* close the connection */
    }

#ifdef USE_SSL

#endif
    /* regular startup packet, cancel, etc packet should follow... */
    /* but not another SSL negotiation request */
    return ProcessStartupPacket(true);
  }

  if (PG_PROTOCOL_MAJOR(proto) < PG_PROTOCOL_MAJOR(PG_PROTOCOL_EARLIEST) ||
  PG_PROTOCOL_MAJOR(proto) > PG_PROTOCOL_MAJOR(PG_PROTOCOL_LATEST)
      || (PG_PROTOCOL_MAJOR(proto) == PG_PROTOCOL_MAJOR(PG_PROTOCOL_LATEST) &&
      PG_PROTOCOL_MINOR(proto) > PG_PROTOCOL_MINOR(PG_PROTOCOL_LATEST))) {
    printf("unsupported frontend protocol %u.%u: server supports %u.0 to %u.%u",
           PG_PROTOCOL_MAJOR(proto), PG_PROTOCOL_MINOR(proto),
           PG_PROTOCOL_MAJOR(PG_PROTOCOL_EARLIEST),
           PG_PROTOCOL_MAJOR(PG_PROTOCOL_LATEST),
           PG_PROTOCOL_MINOR(PG_PROTOCOL_LATEST));
    return false;
  }

  if (PG_PROTOCOL_MAJOR(proto) < 3) {
    // old version ...
    return false;
  }

  char *buf = read_msg_.Data();
  if (PG_PROTOCOL_MAJOR(proto) >= 3) {
    int32_t offset = sizeof(ProtocolVersion);

    /*
     * Scan packet body for name/option pairs.  We can assume any string
     * beginning within the packet body is null-terminated, thanks to
     * zeroing extra byte above.
     */

    while (offset < len) {
      char *nameptr = ((char *) buf) + offset;
      int32_t valoffset;
      char *valptr;

      if (*nameptr == '\0')
        break; /* found packet terminator */
      valoffset = offset + strlen(nameptr) + 1;
      if (valoffset >= len)
        break; /* missing value, will complain below */
      valptr = ((char *) buf) + valoffset;

      if (strcmp(nameptr, "database") == 0)
        database_name_ = valptr;
      else if (strcmp(nameptr, "user") == 0)
        user_name_ = valptr;
      else if (strcmp(nameptr, "options") == 0)
        cmdline_options_ = valptr;
      else if (strcmp(nameptr, "replication") == 0) {
        // DO NOT SUPPORT ...
      } else {
        /* Assume it's a generic GUC option */
      }
      guc_options_.push_back(
          std::make_pair<std::string, std::string>(nameptr, valptr));
      offset = valoffset + strlen(valptr) + 1;
    }

    /*
     * If we didn't find a packet terminator exactly at the end of the
     * given packet length, complain.
     */
    if (offset != len - 1)
      return false;
  }
  proto_version_ = proto;
  return SendAuthRequest();;
}

bool Session::ReadCommand() {
  char qtype;
  if (boost::asio::read(socket_, boost::asio::buffer(&qtype, 1)) != 1) {
    return false;
  }

  /*
   * Validate message type code before trying to read body; if we have lost
   * sync, better to say "command unknown" than to run out of memory because
   * we used garbage as a length word.
   *
   * This also gives us a place to set the doing_extended_query_message flag
   * as soon as possible.
   */
  switch (qtype) {
    case 'Q': /* simple query */
      if (PG_PROTOCOL_MAJOR(proto_version_) < 3) {

      }
      break;

    case 'F': /* fastpath function call */
      if (PG_PROTOCOL_MAJOR(proto_version_) < 3) {
      }
      break;

    case 'X': /* terminate */
      break;

    case 'B': /* bind */
    case 'C': /* close */
    case 'D': /* describe */
    case 'E': /* execute */
    case 'H': /* flush */
    case 'P': /* parse */
      /* these are only legal in protocol 3 */
      if (PG_PROTOCOL_MAJOR(proto_version_) < 3) {
      }
      break;

    case 'S': /* sync */
      /* stop any active skip-till-Sync */
      break;

    case 'd': /* copy data */
    case 'c': /* copy done */
    case 'f': /* copy fail */
      break;

    default:

      /*
       * Otherwise we got garbage from the frontend.  We treat this as
       * fatal because we have probably lost message boundary sync, and
       * there's no good way to recover.
       */

      break;
  }

  if (!ReadBody()) {
    return false;
  }
  /*
   * In protocol version 3, all frontend messages have a length word next
   * after the type code; we can read the message contents independently of
   * the type.
   */
  if (PG_PROTOCOL_MAJOR(proto_version_) >= 3) {

  }
  return true;
}

bool Session::ReadBody() {
  int32_t len;
  if (boost::asio::read(socket_, boost::asio::buffer(&len, sizeof(len))) != 1) {
    return false;
  }
  len = ntohl(len);
  len -= 4;
  if (len < 0) {
    return false;
  }
  read_msg_.ReAlloc(len);
  read_msg_.SetSize(len);
  size_t size = boost::asio::read(
      socket_, buffer(read_msg_.Data(), read_msg_.GetSize()));
  if (size < len) {
    return false;
  }

  return true;
}

bool Session::SendAuthRequest() {
  char msg = 'R';
  uint32_t auth = AUTH_REQ_OK;
  uint32_t len = 8;
  len = htonl(len);
  auth = htonl(auth);
  write_msg_.SetSize(0);
  write_msg_.Append(&msg, 1);
  write_msg_.Append(&len, sizeof(len));
  write_msg_.Append(&auth, sizeof(auth));

  size_t size = boost::asio::write(
      socket_, buffer(write_msg_.Data(), write_msg_.GetSize()));
  if (size != write_msg_.GetSize()) {
    return false;
  }
  return true;
}

bool Session::SendParameterStatus() {
  write_msg_.SetSize(0);
  for (int i = 0; i < guc_options_.size(); i++) {
    char msg = PARAMETER_STATUS;
    write_msg_.Append(&msg, 1);
    std::string & name = guc_options_[i].first;
    std::string & value = guc_options_[i].first;
    uint32_t len = 4 + name.size() + 1 + value.size() + 1;
    len = htonl(len);
    write_msg_.Append(&len, 4);
    write_msg_.Append(name.c_str(), name.size() + 1);
    write_msg_.Append(value.c_str(), value.size() + 1);
  }
  size_t size = boost::asio::write(
      socket_, buffer(write_msg_.Data(), write_msg_.GetSize()));
  if (size != write_msg_.GetSize()) {
    return false;
  }
  return true;
}

// status is READY_FOR_QUERY_IDLE or READY_FOR_QUERY_TRANSACTION_BLOCK or
// READY_FOR_QUERY_TRANSACTION_FAIL
void Session::SendReadForQuery(char status) {
  char msg = READ_FOR_QUERY;
  uint32_t len = 4 + 1;
  len = htonl(len);
  write_msg_.SetSize(0);
  write_msg_.Append(&msg, 1);
  write_msg_.Append(&len, sizeof(len));
  write_msg_.Append(&status, 1);
  Send(write_msg_.Data(), write_msg_.GetSize());
}

void Session::Send(void *data, size_t length) {
  size_t size = boost::asio::write(
      socket_, buffer(write_msg_.Data(), write_msg_.GetSize()));
  if (size != write_msg_.GetSize()) {
    assert(false);
  }
}

void Session::SendBackendKeyData() {
  char msg = BACKEND_KEY_DATA;
  uint32_t len;
  uint32_t pid = 0;
  uint32_t key = 0;
  len = sizeof(len) + sizeof(pid) + sizeof(key);
  len = htonl(len);
  pid = htonl(pid);
  key = htonl(key);
  write_msg_.SetSize(0);
  write_msg_.Append(&msg, 1);
  write_msg_.Append(&len, sizeof(len));
  write_msg_.Append(&pid, sizeof(pid));
  write_msg_.Append(&key, sizeof(key));
  Send(write_msg_.Data(), write_msg_.GetSize());
}

void Session::SendRowDescription(const RowDesc *row_desc) {
  BackendMsgBegin(ROW_DESCRIPTION);
  int16_t columns_count = (int16_t) row_desc->GetColumnCount();
  BackendMsgAppendInt16(columns_count);
  for (int16_t i = 0; i < columns_count; i++) {
    const ColumnDesc & col_desc = row_desc->GetColumnDesc(i);
    // The field name
    BackendMsgAppendString(col_desc.column_name_);
    // The object ID of the table
    BackendMsgAppendInt32(col_desc.relation_id_);
    // The attribute number of the column
    BackendMsgAppendInt16(col_desc.attribute_id_);
    // The object ID of the field's data type
    BackendMsgAppendInt32(col_desc.data_type_);
    // The data type size
    BackendMsgAppendInt16((int16_t) col_desc.data_type_size_);

    // The type modifier. The meaning of the modifier is type-specific.
    BackendMsgAppendInt32(0);

    int16_t format_code = 0;
    if (col_desc.data_type_ != kDTVarchar)
      format_code = 1;
    //The format code being used for the field. Currently will be zero (text) or one (binary).
    BackendMsgAppendInt16(format_code);
  }
  BackendMsgEnd(ROW_DESCRIPTION);
}


void Session::SendRowData(const TupleRow *tuple_row, const RowDesc *desc,
                          const std::vector<uint32_t> & proj_mapping) {
  BackendMsgBegin(DATA_ROW);
  uint16_t number = (uint16_t) proj_mapping.size();
  for (uint16_t i = 0; i < number; i++)
  {
    const ColumnDesc & col_desc = desc->GetColumnDesc(proj_mapping[i]);
    uint32_t nth_tuple = col_desc.item_slot_.nth_tuple_;
    uint32_t nth_item = col_desc.item_slot_.nth_item_;
    const Tuple *tuple = tuple_row->GetTuple(nth_tuple);
    const Slot * slot = tuple->GetSlot(nth_item);
    switch(col_desc.data_type_)
    {
      case kDTInteger:
      case kDTDate:
        BackendMsgAppendInt64(tuple->GetInteger(slot->offset_));
      case kDTFloat:
      case kDTVarchar:
        BackendMsgAppendString(std::string((const char*)tuple->GetValue(slot->offset_), slot->length_));
    }
    BackendMsgAppend(tuple->GetValue(slot->offset_), slot->length_);
  }
  BackendMsgEnd(DATA_ROW);
}

void Session::BackendMsgBegin(int8_t msg_id) {
  // 1 byte message id,  4 byte message body
  write_msg_.SetSize(1 + 4);
  write_msg_.Append(&msg_id, 1);
}

void Session::BackendMsgAppendInt16(int16_t value) {
  value = htons(value);
  write_msg_.Append(&value, sizeof(value));
}

void Session::BackendMsgAppendInt32(int32_t value) {
  value = htonl(value);
  write_msg_.Append(&value, sizeof(value));
}

void Session::BackendMsgAppendInt64(int64_t value) {
  value = htonll(value);
  write_msg_.Append(&value, sizeof(value));
}

void Session::BackendMsgAppendString(const std::string & value) {
  write_msg_.Append(value.c_str(), value.size() + 1);
}

void Session::BackendMsgAppend(const void *value, size_t length) {
  write_msg_.Append(value, length);
}

void Session::BackendMsgEnd(int8_t msg_id) {
  // minus the first byte of the message
  uint32_t length = write_msg_.GetSize() - 1;
  length = htonl(length);
  memcpy(write_msg_.Data() + 1, &length, sizeof(length));
  Send(write_msg_.Data(), write_msg_.GetSize());
  assert(*(uint8_t* )write_msg_.Data() == msg_id);
}

//----------------------------------------------------------------------

