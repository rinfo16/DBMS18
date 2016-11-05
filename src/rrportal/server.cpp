// Modified from boost asio example
// http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <list>
#include <boost/asio.hpp>
#include <memory>
#include "storage/storage_service_interface.h"
#include "session.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<MessageBuffer> MessageQueue;

class Server {
 public:
  Server(boost::asio::io_service& io_service, const tcp::endpoint& endpoint)
      : acceptor_(io_service, endpoint),
        socket_(io_service) {
    do_accept();
  }

 private:
  void do_accept() {
    acceptor_.async_accept(socket_, [this](boost::system::error_code ec)
    {
      if (!ec)
      {
        std::make_shared<Session>(std::move(socket_), connection_manager_)->start();
      }

      do_accept();
    });
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;
  ConnectionManager connection_manager_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[]) {
  try {
    if (argc < 2) {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    boost::asio::io_service io_service;

    std::list<Server> servers;
    for (int i = 1; i < argc; ++i) {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(io_service, endpoint);
    }
    storage::StorageServiceInterface::Instance()->Start();
    io_service.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
