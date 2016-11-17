#ifndef ROW_OUTPUT_INTERFACE_H_
#define ROW_OUTPUT_INTERFACE_H_

#include "common/row_desc.h"
#include "common/tuple_row.h"
namespace realizer {
class RowOutputInterface {
public:
	virtual void SendRowDescription(const RowDesc *row_desc) = 0;
	virtual void SendRowData(const TupleRow *tuple_row,
			const RowDesc *desc) = 0;
	virtual ~RowOutputInterface() {
	}
	;
};
}
#endif // ROW_OUTPUT_INTERFACE_H_
