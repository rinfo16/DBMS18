#ifndef CONNECTION_H__
#define CONNECTION_H__

#include <set>
#include <utility>
#include <deque>
#include <memory>
#include <boost/asio.hpp>
#include "common/message.h"
//----------------------------------------------------------------------

typedef std::deque<MessageBuffer> MessageQueue;

//----------------------------------------------------------------------

class Connection {
 public:
  virtual ~Connection() {
  }
  virtual void deliver(const MessageBuffer& msg) = 0;
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

  void deliver(const MessageBuffer& msg) {
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

#endif // CONNECTION_H__
