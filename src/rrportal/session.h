#ifndef SESSION_H__
#define SESSION_H__

#include <boost/asio.hpp>
#include <vector>
#include <string>
#include "common/message.h"
#include "common/row_desc.h"
#include "connection.h"
#include "common/tuple_row.h"
#include "common/row_desc.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

class Session : public Connection, public std::enable_shared_from_this<Session> {
 public:
  Session(tcp::socket socket, ConnectionManager& manager);

  void start();

  void deliver(const MessageBuffer& msg);

 private:
  bool ProcessStartupPacket(bool ssl_done);
  bool SendAuthRequest();
  bool SendParameterStatus();
  void SendReadForQuery(char status);
  void SendBackendKeyData();
  void SendRowDescription(const RowDesc *row_desc);
  void SendRowData(const TupleRow *tuple_row, const RowDesc *desc,
                   const std::vector<uint32_t> & proj_mapping);
  bool ReadCommand();
  bool ReadBody();

  void do_read_header();
  void do_read_body();
  void do_write();

  void Send(void *data, size_t length);

  void BackendMsgBegin(int8_t msg_id);
  void BackendMsgAppendInt16(int16_t value);
  void BackendMsgAppendInt32(int32_t value);
  void BackendMsgAppendInt64(int64_t value);
  void BackendMsgAppendString(const std::string & value);
  void BackendMsgAppend(const void *value, size_t length);
  void BackendMsgEnd(int8_t msg_id);

  tcp::socket socket_;
  ConnectionManager& manager_;
  MessageBuffer read_msg_;
  MessageBuffer write_msg_;
  MessageQueue write_msgs_;
  std::string database_name_;
  std::string user_name_;
  std::string cmdline_options_;
  std::vector<std::pair<std::string, std::string> > guc_options_;
  ProtocolVersion proto_version_;
};

//----------------------------------------------------------------------

#endif // SESSION_H__

