#ifndef IO_OBJECT_INTERFACE_H_
#define IO_OBJECT_INTERFACE_H_

namespace storage {

class IOObjectInterface : public boost::noncopyable {
 public:
  virtual ~IOObjectInterface() {};
};

}  // namespace storage

#endif // IO_OBJECT_INTERFACE_H_
