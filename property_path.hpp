#ifndef UTILITIES_PROPERTY_PATH_HPP
#define UTILITIES_PROPERTY_PATH_HPP

#include "type_definitions.hpp"
#include "string.hpp"
#include "templates.hpp"

namespace Utilities
{
	struct property_path
	{
		enum class type : int8_t
		{
			// nothing
			Object = 0,
			// <key>:[]
			Collection = 1
		};

		template<typename TRoot, typename TResult>
		struct __get
		{
			TResult& operator()(property_path const& pp, TRoot& root)
			{
				assert(false, "property_path::__get is not specialized");
				throw;
			}
		};

		template<typename TRoot, typename T>
		struct __set
		{
			void operator()(property_path const& pp, TRoot& root, T& value)
			{
				assert(false, "property_path::__set is not specialized");
				throw;
			}
		};

		struct entry
		{
			std::string Key;
			type Type;

			entry(std::string_view element)
			{
				auto splitten = string_split(element.data(), ":");
				switch (splitten.size())
				{
				case(1):
					Key = element;
					Type = type::Object;
					break;
				case(2):
					Key = splitten.front();
					if (splitten.back() == "[]")
					{
						Type = type::Collection;
						break;
					}
					throw std::invalid_argument{ "invalid <type> value" };
				default:
					throw std::invalid_argument{"format: <key> or <key>:<type>"};
				}
			}

			static std::list<entry> parse(std::string_view path)
			{
				std::list<entry> p;
				for (auto& elem : string_split(path.data(), "."))
					p.emplace_back(elem.c_str());
				return p;
			}
		};

		std::string const FullPath;
		std::list<entry> const Path;

		property_path(std::string_view path) :
			FullPath(path), Path(entry::parse(path))
		{}

		template<typename TRoot, typename TResult>
		TResult& get(TRoot& root) const { return __get()(*this, root); }

		template<typename TRoot, typename T>
		void set(TRoot& root, T& value) const { __set()(*this, root, value); }
	};
}

#endif //UTILITIES_PROPERTY_PATH_HPP