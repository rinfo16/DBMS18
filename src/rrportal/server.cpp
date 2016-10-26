// Modified from boost asio example
// http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "common/message.hpp"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<Message> MessageQueue;

//----------------------------------------------------------------------

class Connection {
 public:
  virtual ~Connection() {
  }
  virtual void deliver(const Message& msg) = 0;
};

typedef std::shared_ptr<Connection> ConnectionPtr;

//----------------------------------------------------------------------

class ConnectionManager {
 public:
  void join(ConnectionPtr connection_ptr) {
    participants_.insert(connection_ptr);
    for (auto msg : recent_msgs_)
      connection_ptr->deliver(msg);
  }

  void leave(ConnectionPtr participant) {
    participants_.erase(participant);
  }

  void deliver(const Message& msg) {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant : participants_)
      participant->deliver(msg);
  }

 private:
  std::set<ConnectionPtr> participants_;
  enum {
    max_recent_msgs = 100
  };
  MessageQueue recent_msgs_;
};

//----------------------------------------------------------------------

class Session : public Connection, public std::enable_shared_from_this<Session> {
 public:
  Session(tcp::socket socket, ConnectionManager& room)
      : socket_(std::move(socket)),
        room_(room) {
  }

  void start() {
    room_.join(shared_from_this());
    do_read_header();
  }

  void deliver(const Message& msg) {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) {
      do_write();
    }
  }

 private:
  void do_read_header() {
    auto self(shared_from_this());
    boost::asio::async_read(
        socket_, boost::asio::buffer(read_msg_.data(), Message::header_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }

  void do_read_body() {
    auto self(shared_from_this());
    boost::asio::async_read(
        socket_, boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            room_.deliver(read_msg_);
            do_read_header();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }

  void do_write() {
    auto self(shared_from_this());
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(write_msgs_.front().data(),
                            write_msgs_.front().length()),
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
            room_.leave(shared_from_this());
          }
        });
  }

  tcp::socket socket_;
  ConnectionManager& room_;
  Message read_msg_;
  MessageQueue write_msgs_;
};

//----------------------------------------------------------------------

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
        std::make_shared<Session>(std::move(socket_), room_)->start();
      }

      do_accept();
    });
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;
  ConnectionManager room_;
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

    io_service.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
