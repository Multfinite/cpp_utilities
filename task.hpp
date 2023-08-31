#ifndef UTILITIES_TASK_HPP
#define UTILITIES_TASK_HPP

#include "type_definitions.hpp"
#include "buffer.hpp"
#include "exceptions.hpp"

namespace Utilities
{
	template<typename TIn>
	void convert_parameter(TIn& parameter, _buffer& in);
	template<typename TOut>
	void extract_response(TOut& response, _buffer& out);

	class Task
	{
	public:
		struct execution_error : public Exceptions::base_error
		{
			int Code;

			execution_error(string msg, string function, string file, int line, int code);
		};

	private:
		string			_command;
		void				_run_process			(_buffer& in, _buffer& out);
	public:
		Task(string command);

		template<typename TIn, typename TOut>
		void execute(TIn& parameter, TOut& response)
		{
			_buffer		in, out;

			convert_parameter	<TIn>(parameter, in);
			_run_process(in, out);
			extract_response<TOut>(response, out);
		}
	};

	template<>
	void Task::execute(_buffer& parameter, _buffer& response);
}

#endif // UTILITIES_TASK_HPP