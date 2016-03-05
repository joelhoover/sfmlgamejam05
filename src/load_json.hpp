#ifndef __MYGL_LOAD_JSON_HPP__
#define __MYGL_LOAD_JSON_HPP__

#include <string>
#include "json.hpp"

namespace shrapx
{
	nlohmann::json load_json(const std::string&);

} //namespace mygl

#endif
