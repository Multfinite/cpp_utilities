#ifndef UTILITIES_SERIALIZATION_HPP
#define UTILITIES_SERIALIZATION_HPP

#include "type_definitions.hpp"
#include "property.hpp"
#include "string.hpp"

namespace Utilities::Serialization
{
	using std::any;
	using Utilities::IProperty;
	using Utilities::Property;
	using Utilities::string_split;

	template<typename TContext>
	struct __serialize
	{
		__serialize() = default;
		__serialize(TContext const& ctx) {}

		template<typename ...TProperties>
		void operator()(TContext& context, TProperties&... properties) const
		{
			assert(false, "struct __serialize is not specialized for requred context type!");
			throw;
		}
	};

	template<typename TContext>
	struct __deserialize
	{
		__deserialize() = default;
		__deserialize(TContext const& ctx) {}

		template<typename ...TProperties>
		void operator()(TContext& context, TProperties&... properties) const
		{
			assert(false, "struct __deserialize is not specialized for requred context type!");
			throw;
		}
	};

	template<typename T, typename TContext = std::any>
	struct is_serializable
	{
		using type = T;
		static constexpr auto test(TContext) 	-> decltype(std::declval<T&>().serialize(std::declval<TContext&>()), std::true_type());
		static constexpr std::false_type test(...);		
		using result_type = decltype(test(0));
		static const bool value = result_type::value;
	};

	template<typename T, typename TContext = std::any>
	struct is_deserializable
	{
		using type = T;
		static constexpr auto test(TContext) -> decltype(std::declval<T&>().deserialize(std::declval<TContext const&>()), std::true_type());
		static constexpr std::false_type test(...);
		using result_type = decltype(test(0));
		static const bool value = result_type::value;
	};

#define serializable(...) \
template<typename TContext> \
void serialize(TContext& context) const \
{ \
	Utilities::Serialization::__serialize{ context }(context, \
		__VA_ARGS__ \
	); \
} \

#define deserializable(...) \
template<typename TContext> \
void deserialize(TContext const& context) const \
{ \
	Utilities::Serialization::__deserialize{ context }(context, \
		__VA_ARGS__ \
	); \
} \

#define fully_serializable(...) \
serializable(__VA_ARGS__) \
deserializable(__VA_ARGS__) \


	class ____serialization_property_example
	{
		using self = ____serialization_property_example;
	public:
		property(self, Identifier, "identifier", std::string)

		____serialization_property_example() :
			Identifier(this)
		{	}

		fully_serializable(
			Identifier
		)

		static_assert(is_serializable<____serialization_property_example>::value, "SFINAE is_serializable does not work");
		static_assert(is_deserializable<____serialization_property_example>::value, "SFINAE is_deserializable does not work");

		/*
		template<typename TContext>
		void serialize(TContext& context) const
		{
			__serialize<TContext> { context }(context,
				Identifier
			);
		}
		template<typename TContext>
		void deserialize(TContext const& context) const
		{
			__deserialize<TContext> { context }(context,
				Identifier
			);
		}
		*/
	};
}

#endif //UTILITIES_SERIALIZATION_HPP