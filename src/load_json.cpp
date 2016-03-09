#include "load_json.hpp"

#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>

#include "json.hpp"

// iterate array
//	for (auto a : data)

// if key exists
// data.count("key")
// data["key"]

// get type
// data.get<int>()

namespace shrapx
{

nlohmann::json load_json(const std::string& filename )
{
	std::ifstream file_stream;
	std::stringstream string_stream;

	file_stream.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);

	std::cout << "load: " << filename.c_str() << "." << std::endl;

	try
	{
		file_stream.open(filename.c_str());
		assert( !file_stream.fail() );

		string_stream << file_stream.rdbuf();
		file_stream.close();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unknown Exception!" << std::endl;
	}

	return nlohmann::json::parse(string_stream);
}

} // namespace shrapx
