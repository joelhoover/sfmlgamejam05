#include "load_json.hpp"

#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>

#include "json.hpp"

namespace shrapx
{

nlohmann::json load_json(const std::string& filename )
{
	using ifstream = std::basic_ifstream<char>;
	ifstream file_stream;
	std::stringstream string_stream;

	file_stream.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);

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
};

} // namespace mygl
