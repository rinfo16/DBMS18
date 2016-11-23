#include "row_output.h"

namespace rrportal {

RowOutput::RowOutput(Session *ss) :
		ss_(ss) {
}

void RowOutput::SendRowDescription(const RowDesc *row_desc) {
	ss_->SendRowDescription(row_desc);
}
void RowOutput::SendRowData(const TupleRow *tuple_row, const RowDesc *row_desc) {
	ss_->SendRowData(tuple_row, row_desc);
}
std::string  RowOutput::RecvCopyData(){
  return ss_->RecvCopyData();
}
void RowOutput::SendCopyData(std::string & msg){
  ss_->SendCopyData(msg);
}

void RowOutput::SendCopyInResponse(int32_t columns){
  ss_->SendCopyInResponse(columns);
}
}
