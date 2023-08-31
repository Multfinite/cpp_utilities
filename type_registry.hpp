#ifndef TYPE_REGISTRY_HPP
#define TYPE_REGISTRY_HPP

#include "type_definitions.hpp"
#include "exceptions.hpp"

#include <list>

namespace Utilities
{
	template<typename TInterface>
	class TypeRegistry
	{
	public:
		using interface_type	= TInterface;
		using interface_item	= interface_type*;
		using type_code			= TInterface::type_code;
	
		template<typename TIdentifierType>
		struct type_not_found : public Exceptions::base_error
		{
			TIdentifierType Identifier;
	
			type_not_found(string function, string file, int line, TIdentifierType identifier) : base_error(function, file, line), Identifier(identifier) {}
			type_not_found(string msg, string function, string file, int line, TIdentifierType identifier) : base_error(msg, function, file, line), Identifier(identifier) {}
			type_not_found(string function, string file, int line) : base_error(function, file, line) {}
			type_not_found(string msg, string function, TIdentifierType file, int line) : base_error(msg, function, file, line) {}
		};
	
		using type_not_found_via_type_code = type_not_found<type_code>;
	private:
		std::list<interface_item> _types{};
	public:
		TypeRegistry() = default;
		~TypeRegistry()
		{
			for (auto item : _types)
				item->dispose();
			_types.clear();
		}
	
		std::list<interface_item> const& get_types() const { return _types; }
	
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief This function register instance of interface implementation via creating new instance.
		*/
		template<class TDerivedType, typename... Args>
		void register_type(Args... args)
		{
			TDerivedType* pItem = new TDerivedType(args...);
			_types.push_back(pItem);
		}
	
		inline interface_item get_instance_via_type_code(type_code typeCode) const
		{
			auto type = std::find_if(_types.begin(), _types.end(), [&typeCode](const interface_item value) -> bool
			{
				return value->get_type() == typeCode;
			});
			if (type == _types.end())
				throw new_base_error_args(type_not_found_via_type_code, "Invalid type code", typeCode);
	
			return *type;
		}
	
		template<typename TObjectTypeIdentifier>
		inline interface_item get_instance_via_object_type(DatabaseConnection& db, TObjectTypeIdentifier objectType) const
		{
			using type_not_found_via_object_type = type_not_found<TObjectTypeIdentifier>;
	
			auto type = std::find_if(_types.begin(), _types.end(), [&objectType, &db](const interface_item value) -> bool
			{
				return value->get_object_type(db) == objectType;
			});
			if (type == _types.end())
				throw new_base_error_args(type_not_found_via_object_type, "Invalid object type", objectType);
	
			return *type;
		}
	};
}
#endif // TYPE_REGISTRY_HPP