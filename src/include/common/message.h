#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "common/pgcomm.h"

#define PARAMETER_STATUS 'S'
#define READ_FOR_QUERY 'Z'
#define BACKEND_KEY_DATA 'K'
#define ROW_DESCRIPTION 'T'
#define DATA_ROW 'D'

#define READY_FOR_QUERY_IDLE 'I'
#define READY_FOR_QUERY_TRANSACTION_BLOCK 'T'
#define READY_FOR_QUERY_TRANSACTION_FAIL 'E'

class MessageBuffer {
 public:
  enum {
    header_length = 4
  };
  enum {
    buffer_length = 512
  };

  MessageBuffer()
      : buffer_length_(0),
        data_length_(0),
        data_(NULL) {
    data_ = (char*) malloc(buffer_length);
  }

  ~MessageBuffer() {
    if (data_) {
      free(data_);
      data_ = NULL;
    }
  }
  void ReAlloc(size_t size) {
    if (size > buffer_length_
        || (size < buffer_length_ * 0.2 && buffer_length_ > buffer_length)) {
      buffer_length_ = size;
      data_ = (char*) realloc(data_, size);
    }
  }

  const char* Data() const {
    return data_;
  }

  char* Data() {
    return data_;
  }

  size_t Capacity() const {
    return buffer_length_;
  }

  size_t GetSize() const {
    return data_length_;
  }

  void Append(const void *data, size_t len) {
    if (data_length_ + len > buffer_length_) {
      buffer_length_ = data_length_ + len;
      buffer_length_ *= 2;
      ReAlloc(buffer_length_);
    }
    memcpy(data_ + data_length_, data, len);
    data_length_ += len;
  }

  void SetSize(size_t size) {
    data_length_ = size;
  }
  bool decode_header() {
    return true;
  }

  void encode_header() {
  }

 private:
  char *data_;
  size_t data_length_;
  size_t buffer_length_;
};

#endif // MESSAGE_HPP
