#ifndef UTILITIES_JSON_HPP
#define UTILITIES_JSON_HPP
#if ENABLE_NLOHMANN_JSON == 1

#include <string>
#include <memory>
#include <optional>

#include <nlohmann/json.hpp>

#include "string.hpp"
#include "exceptions.hpp"

namespace Utilities::SQL
{
	std::string as_sql(const nlohmann::json& v) { return "'" + v.dump() + "'::json"; };
}

namespace Utilities::JSON
{    
	inline std::string get_json_type_name(nlohmann::json::value_t type)
	{
		switch (type)
		{
			case nlohmann::json::value_t::null:
				return "null";
			case nlohmann::json::value_t::object:
				return "object";
			case nlohmann::json::value_t::array:
				return "array";
			case nlohmann::json::value_t::string:
				return "string";
			case nlohmann::json::value_t::boolean:
				return "boolean";
			case nlohmann::json::value_t::binary:
				return "binary";
			case nlohmann::json::value_t::discarded:
				return "discarded";
			case nlohmann::json::value_t::number_integer:
			case nlohmann::json::value_t::number_unsigned:
			case nlohmann::json::value_t::number_float:
			default:
				return "number";
		}
		throw;
	}
	
	inline void validate_json_object(
		const std::string& function, const std::string& file, int line,
		nlohmann::json const& item, 
		std::optional<std::map<std::string, nlohmann::json::value_t>> requiredKeys)
	{
		auto prefix = function + " [" + file + ":" + std::to_string(line) + "]";
		if (!item.is_object())
			throw std::invalid_argument { prefix + " json is not object"};
	
		if (!requiredKeys.has_value())
			return;
	
		for (auto& kvp : requiredKeys.value())
		{
			if (!item.contains(kvp.first))
				throw std::invalid_argument{ prefix + " missing key { " + kvp.first + ":[" + get_json_type_name(kvp.second) + "] }"};
			auto prop = item[kvp.first];
			if(prop.type() != kvp.second)
				throw std::invalid_argument{ prefix + " invalid key type { " + kvp.first + ":[" + get_json_type_name(kvp.second) + "] }" };
		}
	}
	
	template<typename T>
	T json_value_or(nlohmann::json& j, std::string key, T defaultValue)
	{
		if (j.contains(key))
			return (T)j[key];
		return defaultValue;
	}

	template<typename T>
	inline auto json_compatible(T& value) { return value; }

	template<>
	inline auto json_compatible<std::chrono::system_clock::time_point>(std::chrono::system_clock::time_point& value)
	{ 
		return std::chrono::duration_cast<std::chrono::microseconds>(value.time_since_epoch()).count(); 
	}

	template<>
	inline auto json_compatible<  std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>>(std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>& value)
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(value.time_since_epoch()).count();
	}  

	template<typename T>
	inline void _from_json(const std::string& p, const nlohmann::json& j, std::optional<T>& v)
	{
		try
		{	
			if (j.contains(p))
				j.at(p).get_to(v.emplace());
			/*
			json& jk = (json&) j;
			for (auto key : string_split_ret_s(p, "."))
				if (jk.contains(key))
					jk = jk.at(key);
				else 
					return;
			jk.get_to(v.emplace());
			*/
		}
		catch (const std::exception& ex) {}
	}

	template<typename T>
	inline void _to_json(const std::string& p, nlohmann::json& j, const std::optional<T>& v)
	{
		if (v.has_value())
			j[p] = v.value();
	}
}

#define validate_json_object_(item, requiredKeys) Utilities::validate_json_object(__FUNCTION__, __FILE__, __LINE__, item, requiredKeys)

#endif // ENABLE_NLOHMANN_JSON
#endif // UTILITIES_JSON_HPP
