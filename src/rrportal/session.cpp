#include <cstdlib>
#include <map>
#include "connection.h"
#include "session.h"
#include "assert.h"
#include "parser/parser_service_interface.h"
#include "storage/storage_service_interface.h"
#include "executor/seq_scan.h"
#include "parser/table_reference.h"
#include "parser/column_reference.h"
#include "parser/create_stmt.h"

//----------------------------------------------------------------------
using boost::asio::buffer;

Session::Session(tcp::socket socket, ConnectionManager& manager_)
    : socket_(std::move(socket)),
      manager_(manager_),
      storage_(NULL),
      proto_version_(0),
      thread_(NULL),
      state_(kStateOK) {
}

Session::~Session() {
  BOOST_LOG_TRIVIAL(debug) << "delete thread";
  // before go here, the thread is joinable
  thread_->detach();
  delete thread_;
}

void Session::start() {
  manager_.join(shared_from_this());
  BOOST_LOG_TRIVIAL(debug) << "create new thread";
  thread_ = new std::thread(&Session::Thread, this);
}

void Session::Stop() {
  BOOST_LOG_TRIVIAL(debug) << "stop the session.";
  Connection::Stop();
  thread_->join();
  manager_.leave(shared_from_this());
  BOOST_LOG_TRIVIAL(debug) << "session stop.";
}

void Session::Thread() {
  do {
    try {
      storage_ = storage::StorageServiceInterface::Instance();
      if (!ProcessStartupPacket(false)) {
        break;
      }
      if (!SendParameterStatus()) {
        break;
      }
      SendBackendKeyData();
      SendReadForQuery(READY_FOR_QUERY_IDLE);

      MainLoop();
    } catch (const std::runtime_error &ex) {
      BOOST_LOG_TRIVIAL(error) << ex.what() << '\n';
    }
  } while (0);

  if (IsStop() && state_ == kStateFrontendTerminate) {
    BOOST_LOG_TRIVIAL(info) << "session end.";
    // frontend close the connection
    manager_.leave(shared_from_this());
  }
}

void Session::MainLoop() {
  while (!IsStop()) {
    ProcessCommand();
  }
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

bool Session::ProcessCommand() {
  char qtype;
  if (boost::asio::read(socket_, boost::asio::buffer(&qtype, 1)) != 1) {
    return false;
  }

  if (!ReadBody()) {
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
    case 'Q': /* simple query */{
      if (PG_PROTOCOL_MAJOR(proto_version_) < 3) {

      }
      std::string query(read_msg_.Data(), read_msg_.GetSize());
      ProcessSimpleQuery(query);
      break;
    }

    case 'F': /* fastpath function call */
      if (PG_PROTOCOL_MAJOR(proto_version_) < 3) {
      }
      break;

    case 'X': /* terminate */{
      state_ = kStateFrontendTerminate;
      Connection::Stop();
      break;
    }

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
  /*
   * In protocol version 3, all frontend messages have a length word next
   * after the type code; we can read the message contents independently of
   * the type.
   */
  if (PG_PROTOCOL_MAJOR(proto_version_) >= 3) {

  }
  return true;
}

void Session::ProcessSimpleQuery(const std::string & query) {
  BOOST_LOG_TRIVIAL(debug)<< query;
  parser::SQLParserInterface *sql_parser =
      parser::ParserServiceInterface::Instance()->CreateSQLParser(query);
  ASTBase *parse_tree = sql_parser->Parse();
  if (parse_tree == NULL) {
    return;
  }
  if (parse_tree->ASTType() == kASTCreateStmt) {
    CreateStmt *create_stmt = dynamic_cast<CreateStmt *>(parse_tree);
    ProcessCreateTable(create_stmt);
  } else if (parse_tree->ASTType() == kASTCopyStmt) {
    LoadStmt *load_stmt = dynamic_cast<LoadStmt *>(parse_tree);
    ProcessLoadData(load_stmt);
  } else if (parse_tree->ASTType() == kASTSelectStmt) {
    SelectStmt *select_stmt = dynamic_cast<SelectStmt *>(parse_tree);
    ProcessSelect(select_stmt);
  }
  parser::ParserServiceInterface::Instance()->DestroySQLParser(sql_parser);
  SendReadForQuery(READY_FOR_QUERY_IDLE);
}

void Session::ProcessCreateTable(CreateStmt *create_stmt) {
  TableSchema schema;
  schema.name_ = create_stmt->TableName();
  const std::vector<ColumnDefine*> &def_list = create_stmt->ColumnDefineList();
  for (int i = 0; i < def_list.size(); i++) {
    ColumnDefine *column_define = def_list[i];
    ColumnSchema column_schema;
    column_schema.name_ = column_define->ColumnName();
    column_schema.data_type_ = column_define->ColumnType();
    column_schema.length_ = column_define->DataLength();
    column_schema.is_null_ = false;
    schema.column_list_.push_back(column_schema);
  }
  bool ok = storage_->CreateRelation(schema);
  if (ok) {
    SendCommandComplete("table " + schema.name_ + " created.");
  } else {
    SendErrorResponse("table " + schema.name_ + " create failed.");
  }
}

void Session::ProcessLoadData(LoadStmt *load_stmt) {
  const std::string & table_name = load_stmt->TableName();
  const std::string & file_path = load_stmt->FilePath();
  bool ok = storage_->Load(table_name, file_path);
  std::string msg;
  if (ok) {
    msg = "copy " + table_name + " from '" + file_path + "' success.";
    SendCommandComplete(msg);
  } else {
    msg = "copy " + table_name + " from '" + file_path + "' failed.";
    SendErrorResponse(msg);
  }
}

bool Session::ProcessSelect(SelectStmt *select_stmt) {
// TODO .. semantic check
// TODO .. reduce sql
// TODO .. optimize path
// TODO .. CBO
// TODO .. move the follow code to realizer or executor buidler ...
  bool ret = false;
  storage::IteratorInterface *iter = NULL;
  std::string table_name;
  auto table_factor_list = select_stmt->TableReference();
  const TableFactor * table_factor = table_factor_list[0];
  if (table_factor->ASTType() == kASTTableReference) {
    const TableReference *table_reference =
        dynamic_cast<const TableReference*>(table_factor);
    table_name = table_reference->TableName();
  } else {
    // NOT SUPPORT TODO
    assert(false);
  }

  Relation *relation = storage_->GetMetaDataManager()->GetRelationByName(
      table_name);
  if (relation == NULL) {
    return false;
  }
  std::vector<uint32_t> projection_mapping;
  auto select_list = select_stmt->SelectList();
  for (size_t i = 0; i < select_list.size(); i++) {
    const ExpressionBase *expr = select_list[i]->SelectExpression();
    if (expr->ASTType() == kASTColumnReference) {
      const ColumnReference *column_reference =
          dynamic_cast<const ColumnReference*>(expr);
      int32_t index = relation->GetAttributeIndex(
          column_reference->ColumnName());
      if (index < 0)
        return false;
      projection_mapping.push_back(index);
    }
  }

  iter = storage_->NewIterator(table_name);
  if (iter == NULL) {
    return false;
  }
  RowDesc desc = relation->ToDesc();
  BOOST_LOG_TRIVIAL(debug)<< "build execution tree";
// begin build execution tree ...

  TupleRow *row = NULL;
  SendRowDescription(&desc, projection_mapping);
  executor::ExecInterface *exec = new executor::SeqScan(iter, 0);

  std::stringstream ssm;
  int rows = 0;
  BOOST_LOG_TRIVIAL(debug) << "execute the plan";
// execute the plan
  if (!exec->Prepare()) {
    goto RET;
  }
  if (!exec->Open()) {
    goto RET;
  }

  row = new TupleRow(1);
  while (true) {
    bool ok = exec->GetNext(row);
    if (!ok)
      break;
    rows++;
    SendRowData(row, &desc, projection_mapping);
    free(row->GetTuple(0));
  }
  exec->Close();
  ssm << "SELECT " << rows << " rows." << std::endl;
  SendCommandComplete(ssm.str());
  ret = true;

  RET: delete row;
  delete exec;
  storage_->DeleteIOObject(iter);
  return ret;
}

bool Session::ReadBody() {
  int32_t len;
  if (boost::asio::read(socket_, boost::asio::buffer(&len, sizeof(len)))
      != sizeof(len)) {
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

void Session::SendRowDescription(const RowDesc *row_desc,
                                 const std::vector<uint32_t> & proj_mapping) {
  BackendMsgBegin(ROW_DESCRIPTION);
  int16_t columns_count = (int16_t) proj_mapping.size();
  BackendMsgAppendInt16(columns_count);
  for (int16_t i = 0; i < columns_count; i++) {
    const ColumnDesc & col_desc = row_desc->GetColumnDesc(proj_mapping[i]);
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
    BackendMsgAppendInt32(-1);

    int16_t format_code = 0;
    //The format code being used for the field. Currently will be zero (text) or one (binary).
    BackendMsgAppendInt16(format_code);
  }
  BackendMsgEnd(ROW_DESCRIPTION);
}

void Session::SendRowData(const TupleRow *tuple_row, const RowDesc *desc,
                          const std::vector<uint32_t> & proj_mapping) {
  BackendMsgBegin(DATA_ROW);
  uint16_t number = (uint16_t) proj_mapping.size();

// The number of column values that follow (possibly zero).
  BackendMsgAppendInt16(number);
  for (uint16_t i = 0; i < number; i++) {
    const ColumnDesc & col_desc = desc->GetColumnDesc(proj_mapping[i]);
    uint32_t nth_tuple = col_desc.item_slot_.nth_tuple_;
    uint32_t nth_item = col_desc.item_slot_.nth_item_;

    const Tuple *tuple = tuple_row->GetTuple(nth_tuple);
    const Slot * slot = tuple->GetSlot(nth_item);
    // The length of the column value, in bytes (this count does not include
    // itself). Can be zero. As a special case, -1 indicates a NULL column
    // value. No value bytes follow in the NULL case

    uint16_t off = slot->offset_;
    uint16_t len = slot->length_;
    std::string value;
    switch (col_desc.data_type_) {
      case kDTInteger:
      case kDTDate: {
        std::stringstream ssm;
        int64_t i = tuple->GetInteger(off);
        ssm << i;
        value = ssm.str();
      }
        break;
      case kDTFloat:
        break;
      case kDTVarchar:
        value = std::string((const char*) tuple->GetValue(off), len);
        break;
      default:
        break;
    }
    int32_t length = value.size();
    BackendMsgAppendInt32(length + 1);
    BackendMsgAppendString(value);
  }
  BackendMsgEnd(DATA_ROW);
}

void Session::SendCommandComplete(const std::string & msg) {
  BackendMsgBegin(COMMAND_COMPLETE);
  BackendMsgAppendString(msg);
  BackendMsgEnd(COMMAND_COMPLETE);
}

void Session::SendErrorResponse(const std::string & msg) {
  BackendMsgBegin(ERROR_RESPONSE);
  BackendMsgAppendInt8(1);
  BackendMsgAppendString(msg);
  BackendMsgEnd(ERROR_RESPONSE);
}

void Session::BackendMsgBegin(int8_t msg_id) {
// 1 byte message id,  4 byte message body
  write_msg_.SetSize(0);
  write_msg_.Append(&msg_id, 1);
  write_msg_.SetSize(1 + 4);
}

void Session::BackendMsgAppendInt8(int8_t value) {
  write_msg_.Append(&value, 1);
}

void Session::BackendMsgAppendInt16(int16_t value) {
  value = htons(value);
  write_msg_.Append(&value, sizeof(value));
}

void Session::BackendMsgAppendInt32(int32_t value) {
  value = htonl(value);
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

