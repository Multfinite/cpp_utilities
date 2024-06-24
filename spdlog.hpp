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
                inline void trace(
                        json const& msg,
                        const int indent = -1,
                        const char indent_char = ' ',
                        const bool ensure_ascii = false,
                        const nlohmann::json::error_handler_t error_handler = nlohmann::json::error_handler_t::strict)
		{
                        spdlog::trace(msg.dump(indent, indent_char, ensure_ascii, error_handler));
		}
                inline void debug(
                        json const& msg,
                        const int indent = -1,
                        const char indent_char = ' ',
                        const bool ensure_ascii = false,
                        const nlohmann::json::error_handler_t error_handler = nlohmann::json::error_handler_t::strict)
		{
                        spdlog::debug(msg.dump(indent, indent_char, ensure_ascii, error_handler));
		}
                inline void info(
                        json const& msg,
                        const int indent = -1,
                        const char indent_char = ' ',
                        const bool ensure_ascii = false,
                        const nlohmann::json::error_handler_t error_handler = nlohmann::json::error_handler_t::strict)
		{
                        spdlog::info(msg.dump(indent, indent_char, ensure_ascii, error_handler));
		}
                inline void warn(
                        json const& msg,
                        const int indent = -1,
                        const char indent_char = ' ',
                        const bool ensure_ascii = false,
                        const nlohmann::json::error_handler_t error_handler = nlohmann::json::error_handler_t::strict)
		{
                        spdlog::warn(msg.dump(indent, indent_char, ensure_ascii, error_handler));
		}
                inline void error(
                        json const& msg,
                        const int indent = -1,
                        const char indent_char = ' ',
                        const bool ensure_ascii = false,
                        const nlohmann::json::error_handler_t error_handler = nlohmann::json::error_handler_t::strict)
		{
                        spdlog::error(msg.dump(indent, indent_char, ensure_ascii, error_handler));
		}
                inline void critical(
                        json const& msg,
                        const int indent = -1,
                        const char indent_char = ' ',
                        const bool ensure_ascii = false,
                        const nlohmann::json::error_handler_t error_handler = nlohmann::json::error_handler_t::strict)
		{
                        spdlog::critical(msg.dump(indent, indent_char, ensure_ascii, error_handler));
		}
#endif
	};
}

#endif
