#include "task.hpp"

#include "tiny-process-library/process.hpp"

namespace Utilities
{
	using TinyProcessLib::Config;
	using TinyProcessLib::Process;

	Task::Task(string command)	:
		_command(command)
	{}

	Task::execution_error::execution_error(string msg, string function, string file, int line, int code) :
		base_error(msg, function, file, line),
		Code(code)
	{}

	void Task::_run_process(_buffer& in, _buffer& out)
	{
		stringstream ssErr{}, ssOut{};

		Config		config;
		config.inherit_file_descriptors			= false;

		Process		process		{ _command, "", 
			[&ssOut](const char* bytes, size_t n)
			{
				ssOut.write(bytes, n);
			},
			[&ssErr](const char* bytes, size_t n)
			{
				ssErr.write(bytes, n);
			},
			true, config
		};
		process.write((const char*) in.data(), in.size());

		auto			exitStatus	= process.get_exit_status();
		if (exitStatus != EXIT_SUCCESS)
		{
			size_t			size	= ssErr.tellp();
			string			err; err.resize(size);
			ssErr.read((char*) err.data(), err.size());
			throw construct_error_args(execution_error, err, exitStatus);
		}
		else
		{
			out						= _buffer{ ssOut };
		}
	}

	template<>
	void convert_parameter(string& parameter, _buffer& in)
	{
		size_t	sz			= parameter.size();
		size_t	total		= sz + sizeof(size_t);
		in						= _buffer { total };
		void*		head		= in.push<size_t>(in.get<byte>(), sz);
					head		= in.push(head, parameter);
	}
	template<>
	void extract_response(string& response, _buffer& out)
	{
		size_t	sz			= *out.get<size_t>();
		response.resize(sz);
		auto		pStr		= (const char*) out.offset<size_t>(1);
		memcpy((void*) response.data(), pStr, sz);
	}
	template<>
	void convert_parameter(json& parameter, _buffer& in)
	{
		string data						= parameter.dump();
		convert_parameter<string>(data, in);
	}
	template<>
	void extract_response(json& response, _buffer& out)
	{
		string data;
		extract_response<string>(data, out);
		response						= json::parse(data);
	}

	template<>
	void Task::execute(_buffer& parameter, _buffer& response)
	{
		_run_process(parameter, response);
	}
}