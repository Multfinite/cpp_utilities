#ifndef UTILITIES_SERIALIZATION_JSON_HPP
#define UTILITIES_SERIALIZATION_JSON_HPP

#include "json.hpp"
#include "serialization.hpp"

namespace Utilities::Serialization
{
	template<>
	struct __serialize<json>
	{
		__serialize() = default;
		__serialize(json const& ctx) {}

		template<typename ...TProperties>
		void operator()(json& context, TProperties&... properties) const
		{
			([&]{
				properties.path().set(context, properties.value());
			} (), ...);
		}
	};

	template<>
	struct __deserialize<json>
	{
		__deserialize() = default;
		__deserialize(json const& ctx) {}

		template<typename ...TProperties>
		void operator()(json& context, TProperties&... properties) const
		{
			([&]{
				properties = properties.path().get(context);
			} (), ...);
		}
	};

#define json_serializable_operator \
	operator nlohmann::json() const \
	{ \
		nlohmann::json context{}; \
		serialize(context); \
		return context; \
	} \

#define json_deserializable_operator \
	auto& operator=(const json& rhs)  \
	{ \
		deserialize(rhs); \
		return *this; \
	} \

#define json_fully_serializable_operator \
json_serializable_operator \
json_deserializable_operator \


}

#endif //UTILITIES_SERIALIZATION_JSON_HPP