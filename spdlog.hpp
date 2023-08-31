#ifndef SPDLOG_HPP
#define SPDLOG_HPP

#include "type_definitions.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Utilities
{
	class spdloglib
	{
	public:
	private:
	public:
		spdloglib()
		{
			auto console = spdlog::stdout_color_mt("console");
			auto err_logger = spdlog::stderr_color_mt("stderr");
	
			spdlog::set_pattern("%v", spdlog::pattern_time_type::utc);
			spdlog::set_level(spdlog::level::trace);	
			spdlog::set_default_logger(console);
		}
	
		inline void trace(json& msg)
		{
			auto txt = msg.dump();
			spdlog::trace( txt);
		}
		inline void debug(json& msg)
		{
			auto txt = msg.dump();
			spdlog::debug(txt);
		}
		inline void info(json& msg)
		{
			auto txt = msg.dump();
			spdlog::info(txt);
		}
		inline void warn(json& msg)
		{
			auto txt = msg.dump();
			spdlog::warn(txt);
		}
		inline void construct_error(json& msg)
		{
			auto txt = msg.dump();
			spdlog::error(txt);
		}
		inline void critical(json& msg)
		{
			auto txt = msg.dump();
			spdlog::critical(txt);
		}
	};
}

#endif