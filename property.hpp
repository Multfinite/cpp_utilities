#ifndef UTILITIES_PROPERTY_HPP
#define UTILITIES_PROPERTY_HPP

#include "events.hpp"

namespace Utilities
{
	template<typename TOwner, typename TDataType>
	struct IProperty 
	{
		Event<Property<TOwner, TDataType>&> Changed;
	};

	template<typename TOwner, typename TDataType>
	struct Property : IProperty<TOwner, TDataType>
	{
		TOwner* _owner;
		TDataType _value;
		Property(TOwner* owner) : _owner(owner) {}
		Property(TOwner* owner, TDataType& value) : _owner(owner), _value(value) {}
		Property(TOwner* owner, TDataType* value) : _owner(owner), _value(*value) {}
		~Property() = default;

		operator=(TDataType& rhs) { _value = rhs; }
		operator!=(TDataType& rhs) const { return _value != rhs; }
		operator==(TDataType& rhs) const { return _value == rhs; }

		operator TDataType() { return _value; }
		operator TDataType() const { return _value; }
	};
}

#endif //UTILITIES_PROPERTY_HPP