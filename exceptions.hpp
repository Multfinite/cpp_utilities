#ifndef UTILITIES_EXCEPTIONS_HPP
#define UTILITIES_EXCEPTIONS_HPP

#include <string>
#include <string_view>
#include <exception>
#include <stdexcept>

#ifdef UTILITIES_EXCEPTIONS_USE_JSON
#include <nlohmann/json.hpp>
#endif //UTILITIES_EXCEPTIONS_USE_JSON

namespace Exceptions
{	
	/*!
	* @author multfinite
	* @brief Base exception class with some metadata.
	*/
	struct base_error : public std::runtime_error
	{
		std::string		Function;
		std::string		File;
		int					Line;
	
		base_error(std::string function, std::string file, int line) : base_error("An unknown error", function, file, line) {}
		base_error(std::string msg, std::string function, std::string file, int line) :
			std::runtime_error(msg),
			Function(function),
			File(file),
			Line(line)
		{ }
	
#ifdef UTILITIES_EXCEPTIONS_USE_JSON
		inline nlohmann::json format_block() const
		{
			using nlohmann::json;
			using std::string;
	
			json block{};
	
			string msg = what();
	
			block["file"]					= File;
			block["line"]					= Line;
			block["function"]			= Function;
			block["message"]			= msg;
			block["type"]					= typeid(this).name();
	
			return block;
		}
#endif //UTILITIES_EXCEPTIONS_USE_JSON
	};
	
	struct not_implemented_error : public base_error
	{
		not_implemented_error(std::string function, std::string file, int line) : base_error("Function (" + function + ") not implemented", function, file, line) {}
		not_implemented_error(std::string msg, std::string function, std::string file, int line) : base_error(msg, function, file, line) { }
	};
	struct file_not_found_error : public base_error
	{
		std::string const FileName;

		file_not_found_error(std::string function, std::string file, int line, std::string_view const& filename) : base_error("Function (" + std::string(filename) + ") not found", function, file, line), FileName(filename) {}
		file_not_found_error(std::string msg, std::string function, std::string file, int line, std::string_view const& filename) : base_error(msg, function, file, line), FileName(filename) { }
	};
}

#define construct_error(type, msg) type { msg, __FUNCTION__, __FILE__, __LINE__ }
#define construct_error_no_msg(type) type { __FUNCTION__, __FILE__, __LINE__ }
#define construct_error_args(type, msg, ...) type { msg, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ }
#define construct_error_args_no_msg(type, ...) type { __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ }

#endif // !UTILITIES_EXCEPTIONS_HPP
