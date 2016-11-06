#ifndef SESSION_H__
#define SESSION_H__

#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <thread>
#include "connection.h"
#include "common/message.h"
#include "common/row_desc.h"
#include "common/tuple_row.h"
#include "common/row_desc.h"
#include "parser/select_stmt.h"
#include "parser/create_stmt.h"
#include "parser/load_stmt.h"
#include "storage/storage_service_interface.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

class Session : public Connection, public std::enable_shared_from_this<Session> {
 public:
  Session(tcp::socket socket, ConnectionManager& manager);
  ~Session();
  void start();
  void Thread();
  void Stop();
  void deliver(const MessageBuffer& msg);

  void MainLoop();

 private:
  bool ProcessStartupPacket(bool ssl_done);bool SendAuthRequest();bool SendParameterStatus();
  void SendReadForQuery(char status);
  void SendBackendKeyData();
  void SendRowDescription(const RowDesc *row_desc,
                          const std::vector<uint32_t> &proj_mapping);
  void SendRowData(const TupleRow *tuple_row, const RowDesc *desc,
                   const std::vector<uint32_t> & proj_mapping);
  void SendCommandComplete(const std::string & msg);
  void SendErrorResponse(const std::string & msg);

  bool ProcessCommand();
  void ProcessSimpleQuery(const std::string & query);
  void ProcessCreateTable(CreateStmt *create_stmt);
  void ProcessLoadData(LoadStmt *load_stmt);bool ProcessSelect(
      SelectStmt *select_stmt);bool ReadBody();

  void do_read_header();
  void do_read_body();
  void do_write();

  void Send(void *data, size_t length);

  void BackendMsgBegin(int8_t msg_id);
  void BackendMsgAppendInt8(int8_t value);
  void BackendMsgAppendInt16(int16_t value);
  void BackendMsgAppendInt32(int32_t value);
  void BackendMsgAppendString(const std::string & value);
  void BackendMsgAppend(const void *value, size_t length);
  void BackendMsgEnd(int8_t msg_id);

  tcp::socket socket_;
  ConnectionManager& manager_;
  storage::StorageServiceInterface *storage_;
  MessageBuffer read_msg_;
  MessageBuffer write_msg_;
  MessageQueue write_msgs_;
  std::string database_name_;
  std::string user_name_;
  std::string cmdline_options_;
  std::vector<std::pair<std::string, std::string> > guc_options_;
  ProtocolVersion proto_version_;
  std::thread *thread_;
};

//----------------------------------------------------------------------

#endif // SESSION_H__

