#ifndef UTILITIES_EXCEPTIONS_HPP
#define UTILITIES_EXCEPTIONS_HPP

#include <string>
#include <string_view>
#include <exception>
#include <stdexcept>

#if ENABLE_NLOHMANN_JSON == 1
#include <nlohmann/json.hpp>
#endif //ENABLE_NLOHMANN_JSON

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
	
		base_error(const std::string& function, const std::string& file, int line) : base_error("An unknown error", function, file, line) {}
		base_error(const std::string& msg, const std::string& function, const std::string& file, int line) :
			std::runtime_error(msg),
			Function(function),
			File(file),
			Line(line)
		{ }
	
#if ENABLE_NLOHMANN_JSON == 1
		inline nlohmann::json format_block() const
		{
			using nlohmann::json;
			using std::string;
	
			json block{};
	
			string msg = what();
	
			block["file"] = File;
			block["line"] = Line;
			block["function"] = Function;
			block["message"] = msg;
			block["type"] = typeid(this).name();
	
			return block;
		}
#endif //ENABLE_NLOHMANN_JSON
	};
	
	struct not_implemented_error : public base_error
	{
		not_implemented_error(const std::string& function, const std::string& file, int line) : base_error("Function (" + function + ") not implemented", function, file, line) {}
		not_implemented_error(const std::string& msg, const std::string& function, const std::string& file, int line) : base_error(msg, function, file, line) { }
	};
	struct file_not_found_error : public base_error
	{
		std::string const FileName;

		file_not_found_error(const std::string& function, const std::string& file, int line, std::string_view const& filename) : base_error("File (" + std::string(filename) + ") not found", function, file, line), FileName(filename) {}
		file_not_found_error(const std::string& msg, const std::string& function, std::string file, int line, std::string_view const& filename) : base_error(msg, function, file, line), FileName(filename) { }
	};
	struct item_not_found_exception : public base_error
	{
		item_not_found_exception(const std::string& function, const std::string& file, int line) : base_error("Item not found.", function, file, line) { }
		item_not_found_exception(const std::string& msg, const std::string& function, const std::string& file, int line) : base_error(msg, function, file, line) { }
	};
        struct parse_error : public base_error
        {
                parse_error(const std::string& function, const std::string& file, int line) : base_error("Parsing failed.", function, file, line) { }
                parse_error(const std::string& msg, const std::string& function, const std::string& file, int line) : base_error(msg, function, file, line) { }
        };
}

#define construct_error(type, msg) type { msg, __FUNCTION__, __FILE__, __LINE__ }
#define construct_error_no_msg(type) type { __FUNCTION__, __FILE__, __LINE__ }
#define construct_error_args(type, msg, ...) type { msg, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ }
#define construct_error_args_no_msg(type, ...) type { __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ }

#endif // !UTILITIES_EXCEPTIONS_HPP
