#ifndef QUERY_REALIZER_INTERFACE_H__
#define QUERY_REALIZER_INTERFACE_H__

#include <string>
#include "common/state.h"
#include "realizer/row_output_interface.h"

namespace realizer {

class QueryRealizerInterface {
public:
	virtual ~QueryRealizerInterface() {};
	virtual State Parse() = 0;
	virtual State Check() = 0;
	virtual State Optimize() = 0;
	virtual State Build() = 0;
	virtual State Execute() = 0;
	virtual std::string Message() const = 0;
};

QueryRealizerInterface* NewRealizer(RowOutputInterface *output,
		const std::string&);
void DeleteRealizer(QueryRealizerInterface*);

} // end namespace realizer
#endif // QUERY_REALIZER_INTERFACE_H__
