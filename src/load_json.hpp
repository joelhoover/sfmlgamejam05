#ifndef __LOAD_JSON_HPP__
#define __LOAD_JSON_HPP__

#include <string>
#include "json.hpp"

namespace shrapx
{
	nlohmann::json load_json(const std::string&);
} //namespace shrapx

#endif
