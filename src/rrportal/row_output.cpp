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

}
