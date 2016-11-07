#include "common/define.h"
#include "common/config.h"
#include "server.h"
#include "session.h"
#include "storage/storage_service_interface.h"

bool extern g_quit;
//----------------------------------------------------------------------

Server::Server(uint16_t port)
    : port_(port),
      endpoint_(tcp::v4(), port),
      acceptor_(io_service_, endpoint_),
      socket_(io_service_),
      thread_(NULL) {
}

Server::~Server() {
  delete thread_;
}

void Server::DoAccept() {
  acceptor_.async_accept(
      socket_,
      [this](boost::system::error_code ec)
      {
        if (!ec)
        {
          std::make_shared<Session>(std::move(socket_), connection_manager_)->start();
        }

        DoAccept();
      });
  if (g_quit) {
    Stop();
  }
}

bool Server::Start() {
  bool ok = storage::StorageServiceInterface::Instance()->Start();
  if (!ok) {
    return false;
  }
  BOOST_LOG_TRIVIAL(info) << "storage initialize.";
  DoAccept();
  thread_ = new std::thread(&Server::Run, this);
  BOOST_LOG_TRIVIAL(info) << "server listen on port [" << port_ << "].";
  BOOST_LOG_TRIVIAL(info) << "begin accept connection ...";
  return ok;
}

void Server::Stop() {
  connection_manager_.Stop();
  storage::StorageServiceInterface::Instance()->Stop();
  io_service_.stop();
  thread_->join();
}

void Server::Run() {
  io_service_.run();
}
