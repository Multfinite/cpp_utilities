#ifndef UTILITIES_FILE_HPP
#define UTILITIES_FILE_HPP

#include <string>
#include <list>
#include <memory>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <functional>

#include "exceptions.hpp"

namespace Utilities
{    
	namespace fs = std::filesystem;
	
	using filename_predicate = std::function<bool(fs::directory_entry const& entry)>;

	inline size_t file_size(std::istream& s)
	{
		s.clear(); // Clear EOF bit
		s.seekg(0, std::ifstream::end);
		auto size = static_cast<size_t>(s.tellg());
		s.seekg(0, std::ifstream::beg);
		return size;
	}

	inline std::ifstream file_open_binary(std::string fileName)
	{
		std::ifstream s;
		s.open(fileName.c_str(), std::ios_base::binary);
		if (!s)
			throw construct_error_args_no_msg(Exceptions::file_not_found_error, fileName);
		return s;
	}

	inline std::string file_read_text(std::string fileName, size_t bufferSize = 4096)
	{
		auto stream = std::ifstream(fileName);
		stream.exceptions(std::ios_base::badbit);
	
		if (!stream) 
			throw construct_error_args_no_msg(Exceptions::file_not_found_error, fileName);
	
		auto out = std::string();
		auto buf = std::string(bufferSize, '\0');
		while (stream.read(&buf[0], bufferSize))
			out.append(buf, 0, stream.gcount());        
		out.append(buf, 0, stream.gcount());
		return out;
	}
	
	inline void file_write_text(std::string fileName, std::string text)
	{
	   std:: fstream file{ fileName, std::ios::out | std::ios::trunc };
		file << text;
		file.flush();
		file.close();
	}
	
	inline void file_write(std::string fileName, void* pSrc, size_t size)
	{
		std::fstream file{ fileName, std::ios::out | std::ios::trunc };
		file.write((const char*) pSrc, size);
		file.flush();
		file.close();
	}

	void get_files(fs::path path, std::list<fs::path>& files, bool recursive = true);
	void get_files(fs::path path, std::list<fs::path>& files, filename_predicate predicate, bool recursive = true);
}

#endif // UTILITIES_FILE_HPP