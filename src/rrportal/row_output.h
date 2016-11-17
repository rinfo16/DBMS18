#include "realizer/row_output_interface.h"
#include "session.h"
namespace rrportal {
class RowOutput: public realizer::RowOutputInterface {
public:
	RowOutput(Session *ss);
	virtual ~RowOutput() {
	}
	;
	virtual void SendRowDescription(const RowDesc *row_desc);
	virtual void SendRowData(const TupleRow *tuple_row, const RowDesc *desc);
private:
	Session *ss_;
};
}
