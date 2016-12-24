#include "uuid.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

std::string GenUUID() {
  boost::uuids::basic_random_generator<boost::mt19937> gen;
  return boost::uuids::to_string(gen());
}
