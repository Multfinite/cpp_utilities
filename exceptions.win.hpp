#ifndef UTILITIES_EXCEPTIONS_WIN_HPP
#define UTILITIES_EXCEPTIONS_WIN_HPP

#include "exceptions.hpp"

#include <Windows.h>

namespace Exceptions
{	
	struct winapi_error : public base_error
	{
		DWORD const ErrorCode;

		winapi_error(std::string function, std::string file, int line, DWORD errorCode) : base_error("WINAPI error, last error code: " + std::to_string(errorCode), function, file, line), ErrorCode(errorCode){}
		winapi_error(std::string msg, std::string function, std::string file, int line, DWORD errorCode) : base_error(msg, function, file, line), ErrorCode(errorCode){ }
		
		winapi_error(std::string function, std::string file, int line) : winapi_error(function, file, line, GetLastError()) {}
		winapi_error(std::string msg, std::string function, std::string file, int line) : winapi_error(msg, function, file, line, GetLastError()) {}
	};

	struct load_library_error : winapi_error
	{
		std::string const FileName;

		load_library_error(std::string function, std::string file, int line, std::string_view const& fileName, DWORD errorCode) : winapi_error("LoadLibrary error, last error code" + std::to_string(errorCode), function, file, line, errorCode), FileName(fileName) { }
		load_library_error(std::string msg, std::string function, std::string file, int line, std::string_view const& fileName, DWORD errorCode) : winapi_error(msg, function, file, line, errorCode), FileName(fileName) { }

		load_library_error(std::string function, std::string file, int line, std::string_view const& fileName) : load_library_error(function, file, line, fileName, GetLastError()) { }
		load_library_error(std::string msg, std::string function, std::string file, int line, std::string_view const& fileName) : load_library_error(msg, function, file, line, fileName, GetLastError()) { }
	};

	struct process_creation_error : winapi_error
	{
		process_creation_error(std::string function, std::string file, int line, DWORD errorCode) : winapi_error("Process creation failed, last error code" + std::to_string(errorCode), function, file, line, errorCode) { }
		process_creation_error(std::string msg, std::string function, std::string file, int line, DWORD errorCode) : winapi_error(msg, function, file, line, errorCode) { }

		process_creation_error(std::string function, std::string file, int line) : process_creation_error(function, file, line, GetLastError()) { }
		process_creation_error(std::string msg, std::string function, std::string file, int line) : process_creation_error(msg, function, file, line, GetLastError()) { }
	};
}

#endif //UTILITIES_EXCEPTIONS_WIN_HPP
