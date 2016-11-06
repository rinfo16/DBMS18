#include <iostream>
#include <list>
#include <memory>
#include <deque>
#include <thread>
#include <boost/asio.hpp>
#include "common/message.h"
#include "connection.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<MessageBuffer> MessageQueue;

class Server {
 public:
  Server(uint16_t port);
  ~Server();
  bool Start();
  void Stop();

 private:
  void DoAccept();
  void Run();

  uint16_t port_;

  // take care of the order ...
  // endpoint_ , io_service_ must be initialized before acceptor_
  // io_service_ must be initialized before socket_
  tcp::endpoint endpoint_;
  boost::asio::io_service io_service_;
  tcp::acceptor acceptor_;
  tcp::socket socket_;
  ConnectionManager connection_manager_;
  std::thread *thread_;
};
