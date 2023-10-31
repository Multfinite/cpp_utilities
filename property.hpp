#ifndef UTILITIES_PROPERTY_HPP
#define UTILITIES_PROPERTY_HPP

#include "property_path.hpp"

namespace Utilities
{
	struct IProperty 
	{
		virtual property_path const& path() const = 0;
		virtual void* owner() const = 0;
		virtual void* value() = 0;
	};

	struct __no_metadata {};

#define property_metadata(property_name) property_name##_property_metadata
#define property_metadata_decl(property_name) struct property_metadata(property_name)
#define property_metadata_body(property_name, prop_path) \
struct property_metadata(property_name) \
{ \
	static constexpr const char* __path_cstr = prop_path; \
	static std::optional<Utilities::property_path> __path; \
	static Utilities::property_path const& path() \
	{ \
		if(!__path.has_value()) __path.emplace(__path_cstr); \
		return __path.value(); \
	} \
	static auto& get(decltype(property_name)::owner_type& object) { return object.##property_name; } \
}; \

	template<
		typename TOwner, typename TDataType
		, typename TMetadata = __no_metadata
		, bool = is_iterable<TDataType>::value
	> struct Property;

	template<
		typename TOwner, typename TDataType
		, typename TMetadata
	> struct Property<TOwner, TDataType, TMetadata, false> : public IProperty
	{
		using self_type = Property<TOwner, TDataType, TMetadata>;
		using metadata = TMetadata;
		using owner_type = TOwner;
		using value_type = TDataType;

		property_path const& path() const override 
		{
			if (std::is_same_v<metadata, __no_metadata>)
				throw std::invalid_argument{ "Path must be declared in metadata class as [static property_path path;]. Also, there are no metadata." };
			return metadata::path();
		}

		const owner_type* _owner;
		void* owner() const override { return _owner; }

		value_type _value;
		void* value() override { return &_value; }

		Property(owner_type* owner) : _owner(owner) {}
		Property(owner_type* owner, value_type value) : Property(owner), _value(value) {}
		Property(owner_type* owner, value_type&& value) : Property(owner), _value(value) {}

		~Property() = default;

		self_type& operator=(const value_type& rhs) 
		{
			_value = rhs;
			return *this;
		}
		bool operator!=(const value_type& rhs) const { return _value != rhs; }
		bool operator==(const value_type& rhs) const { return _value == rhs; }

		operator value_type() { return _value; }
		operator value_type() const { return _value; }
		operator value_type&() { return _value; }
		operator value_type&() const { return _value; }
	};

	template<
		typename TOwner, typename TDataType
		, typename TMetadata
	> struct Property<TOwner, TDataType, TMetadata, true> : public IProperty
	{
		using self_type = Property<TOwner, TDataType, TMetadata>;
		using metadata = TMetadata;
		using owner_type = TOwner;
		using value_type = TDataType;

		property_path const& path() const override
		{
			if (std::is_same_v<metadata, __no_metadata>)
				throw std::invalid_argument{ "Path must be declared in metadata class as [static property_path path;]. Also, there are no metadata." };
			return metadata::path();
		}

		const owner_type* _owner;
		void* owner() const override { return _owner; }

		value_type _value;
		void* value() override { return &_value; }

		Property(owner_type* owner) : _owner(owner) {}
		Property(owner_type* owner, value_type value) : Property(owner), _value(value) {}
		Property(owner_type* owner, value_type&& value) : Property(owner), _value(value) {}

		~Property() = default;

		self_type& operator=(const value_type& rhs)
		{
			_value = rhs;
			return *this;
		}
		bool operator!=(const value_type& rhs) const { return _value != rhs; }
		bool operator==(const value_type& rhs) const { return _value == rhs; }

		operator value_type() { return _value; }
		operator value_type() const { return _value; }
		operator value_type&() { return _value; }
		operator value_type&() const { return _value; }

		/* STL container */

		using const_iterator = typename TDataType::const_iterator;
		const_iterator cbegin() const { return _value.cbegin(); }
		const_iterator cend() const { return _value.cend(); }

		using iterator = typename TDataType::iterator;
		iterator begin() { return _value.begin(); }
		iterator end() { return _value.end(); }
	};

#define property(owner_type, property_name, prop_path, data_type) \
	property_metadata_decl(property_name); \
	Property<owner_type, data_type, property_metadata(property_name)> property_name; \
	property_metadata_body(property_name, prop_path) \

}

#endif //UTILITIES_PROPERTY_HPP