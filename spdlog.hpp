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
             inline static std::shared_ptr<spdloglib> Instance;
             inline static std::shared_ptr<spdloglib>& Initialize() { return Instance = std::make_shared<spdloglib>(); }
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
#if ENABLE_NLOHMANN_JSON == 1
                inline void trace(json const& msg)
		{
                        spdlog::trace(msg.dump());
		}
                inline void debug(json const& msg)
		{
                        spdlog::debug(msg.dump());
		}
                inline void info(json const& msg)
		{
                        spdlog::info(msg.dump());
		}
                inline void warn(json const& msg)
		{
                        spdlog::warn(msg.dump());
		}
                inline void error(json const& msg)
		{
                        spdlog::error(msg.dump());
		}
                inline void critical(json const& msg)
		{
                        spdlog::critical(msg.dump());
		}
#endif
	};
}

#endif
