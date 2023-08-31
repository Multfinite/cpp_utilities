#ifndef UTILITIES_LOGGING_HPP
#define UTILITIES_LOGGING_HPP

#include <exception>
#include <string>
#include <chrono>

#include <date/date.h>
#include <nlohmann/json.hpp>

namespace Utilities
{	
	inline nlohmann::json format_response_block(
		std::optional<std::string> content,
		std::chrono::system_clock::time_point time,
		int code,
		std::multimap<std::string, std::string> headers
	)
	{
		using nlohmann::json;
		using std::string;
		using namespace date;
	
		json block{};
		block["code"]				= code;
		block["headers"]		= headers;
		if(content.has_value())
			block["content"]		= content.value();
		block["timestamp"]		= std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch()).count();
	
		return block;
	}
	
	inline nlohmann::json format_endpoint_block(
		string	const& uri,
		json		const& request,
		json		const& response
	)
	{
		using nlohmann::json;
		using std::string;
		using namespace date;
	
		json block{};
	
		block["uri"]				= uri;
		block["request"]			= request;
		block["response"]		= response;
	
		return block;
	}
	
	inline nlohmann::json format_service_block(
		string	const& service,
		string	const& version,
		std::optional<nlohmann::json> context
	)
	{	
		using nlohmann::json;
		using std::string;
		using namespace date;
	
		json block{};
	
		block["service"]			= service;
		block["version"]			= version;
		if(context.has_value() && context.value() != nullptr)
			block["context"]		= context.value();
	
		return block;
	}
	
	inline nlohmann::json format_exception_block(std::exception const& ex)
	{
		using nlohmann::json;
		using std::string;
		using namespace date;
	
		json block{};
	
		block["type"]				= typeid(ex).name();
		block["message"]		= ex.what();
	
		return block;
	}
	
	inline nlohmann::json format_message(
		std::optional<std::string> message,
		std::optional<nlohmann::json> route,
		std::optional<nlohmann::json> service,
		std::optional<nlohmann::json> endpoint,
		std::optional<nlohmann::json> exception,
		std::optional<nlohmann::json> extra,
		std::chrono::system_clock::time_point const& time
	)
	{
		using nlohmann::json;
		using std::string;
		using namespace date;
	
		json block{};
		block["message"]			= message.value_or("");
		block["_timestamp"]		= std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch()).count();
		if (route.has_value() && route.value() != nullptr)
			block["route"]			= route.value();
		if (service.has_value() && service.value() != nullptr)
			block["service"]			= service.value();
		if (endpoint.has_value() && endpoint.value() != nullptr)
			block["endpoint"]		= endpoint.value();
		if (exception.has_value() && exception.value() != nullptr)
			block["exception"]		= exception.value();
		if (extra.has_value() && extra.value() != nullptr)
			block["extra"]			= extra.value();
	
		return block;
	}
}
#endif