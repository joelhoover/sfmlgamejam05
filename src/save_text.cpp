#include "save_text.hpp"

#include <cassert>
#include <exception>
#include <iostream>
#include <string>
#include <fstream>

namespace shrapx
{

void save_text(const std::string& filename, const std::string& data )
{
	std::cout << "save: " << filename << "." << std::endl;

	try
	{
		std::ofstream file_stream(filename);
		assert( !file_stream.fail() );

		file_stream << data.c_str();
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
}

} // namespace shrapx
