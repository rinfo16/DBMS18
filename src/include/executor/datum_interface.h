#ifndef DATUM_INTERFACE_H_
#define DATUM_INTERFACE_H_

#include "common/data_type.h"

namespace executor {
class DatumInterface {
 public:
  DatumInterface()
      : data_type_(kDTBoolean) {
  }
  virtual ~DatumInterface() {
  }
  ;
  DataType GetDataType() const {
    return data_type_;
  }
  void SetDataType(DataType data_type) {
    data_type_ = data_type;
  }
 private:
  DataType data_type_;
};
}
#endif // DATUM_INTERFACE_H_
