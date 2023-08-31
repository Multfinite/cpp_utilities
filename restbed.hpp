#ifndef RESTBED_HPP
#define RESTBED_HPP

//#include <restbed>

#include "corvusoft/restbed/uri.hpp"
//#include "corvusoft/restbed/http.hpp"
#include "corvusoft/restbed/rule.hpp"
#include "corvusoft/restbed/byte.hpp"
#include "corvusoft/restbed/string.hpp"
#include "corvusoft/restbed/logger.hpp"
#include "corvusoft/restbed/request.hpp"
#include "corvusoft/restbed/service.hpp"
#include "corvusoft/restbed/session.hpp"
#include "corvusoft/restbed/response.hpp"
#include "corvusoft/restbed/resource.hpp"
#include "corvusoft/restbed/settings.hpp"
#include "corvusoft/restbed/web_socket.hpp"
#include "corvusoft/restbed/status_code.hpp"
#include "corvusoft/restbed/ssl_settings.hpp"
#include "corvusoft/restbed/context_value.hpp"
#include "corvusoft/restbed/session_manager.hpp"
#include "corvusoft/restbed/web_socket_message.hpp"
#include "corvusoft/restbed/context_placeholder.hpp"
#include "corvusoft/restbed/context_placeholder_base.hpp"

#include <exception>
#include <string>
#include <chrono>
#include <date/date.h>
#include <nlohmann/json.hpp>

namespace Utilities
{
	inline nlohmann::json format_route_block(
		shared_ptr<restbed::Session> const& session
	)
	{
		using nlohmann::json;
		using std::string;
		using namespace date;
	
		json block{};
	
		block["sender"]		= session->get_origin();
		block["recipient"]	= session->get_destination();
	
		return block;
	}
	
	inline nlohmann::json format_request_block(
		std::optional<std::string> content,
		std::chrono::system_clock::time_point time,
		shared_ptr<restbed::Session> const& session
	)
	{
		using nlohmann::json;
		using std::string;
		using namespace date;
	
		json block{};
		block["method"]			= session->get_request()->get_method();
		block["params"]			= session->get_request()->get_path_parameters();
		block["query"]			= session->get_request()->get_query_parameters();
		block["headers"]		= session->get_request()->get_headers();
		if (content.has_value())
			block["content"]		= content.value();
		block["timestamp"]		= std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch()).count();
	
		auto t = block.dump();
		return block;
	}
}

#endif // RESTBED_HPP