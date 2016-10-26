#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "common/message.hpp"

using boost::asio::ip::tcp;

typedef std::deque<Message> MessageQueue;

class Client {
 public:
  Client(boost::asio::io_service& io_service,
         tcp::resolver::iterator endpoint_iterator)
      : io_service_(io_service),
        socket_(io_service) {
    do_connect(endpoint_iterator);
  }

  void write(const Message& msg) {
    io_service_.post([this, msg]()
    {
      bool write_in_progress = !write_msgs_.empty();
      write_msgs_.push_back(msg);
      if (!write_in_progress)
      {
        do_write();
      }
    });
  }

  void close() {
    io_service_.post([this]() {socket_.close();});
  }

 private:
  void do_connect(tcp::resolver::iterator endpoint_iterator) {
    boost::asio::async_connect(
        socket_, endpoint_iterator,
        [this](boost::system::error_code ec, tcp::resolver::iterator)
        {
          if (!ec)
          {
            do_read_header();
          }
        });
  }

  void do_read_header() {
    boost::asio::async_read(
        socket_, boost::asio::buffer(read_msg_.data(), Message::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_read_body() {
    boost::asio::async_read(
        socket_, boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            std::cout.write(read_msg_.body(), read_msg_.body_length());
            std::cout << "\n";
            do_read_header();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_write() {
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(write_msgs_.front().data(),
                            write_msgs_.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
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
            socket_.close();
          }
        });
  }

 private:
  boost::asio::io_service& io_service_;
  tcp::socket socket_;
  Message read_msg_;
  MessageQueue write_msgs_;
};

int main(int argc, char* argv[]) {
  try {
    if (argc != 3) {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    auto endpoint_iterator = resolver.resolve( { argv[1], argv[2] });
    Client c(io_service, endpoint_iterator);

    std::thread t([&io_service]() {io_service.run();});

    char line[Message::max_body_length + 1];
    while (std::cin.getline(line, Message::max_body_length + 1)) {
      Message msg;
      msg.body_length(std::strlen(line));
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      c.write(msg);
    }

    c.close();
    t.join();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
