#ifndef UTILITIES_SQL_HPP
#define UTILITIES_SQL_HPP

#include "type_definitions.hpp"
#include "string.hpp"
#include "templates.hpp"

namespace Utilities::SQL
{
	string select_page(string what, string from, size_t page, size_t perpage)
	{
		string q = "select %what% from %from% limit %limit% offset %offset%";
		string_replace_all_templates(q,
		{
			{ "%what%", what },
			{ "%from%", from },
			{ "%limit%", std::to_string(perpage) },
			{ "%offset%", std::to_string(page * perpage) }
		});
		return q;
	}

	string select_page_with_condition(string what, string from, string condition, size_t page, size_t perpage)
	{
		string q = "select %what% from %from% where %condition% limit %limit% offset %offset%";
		string_replace_all_templates(q,
			{
				{ "%what%", what },
				{ "%from%", from },
				{ "%condition%", condition },
				{ "%limit%", std::to_string(perpage) },
				{ "%offset%", std::to_string(page * perpage) }
			});
		return q;
	}

	string count_rows(string from)
	{
		string q = "select count(*) as rows from %from%";
		string_replace_all_templates(q,
		{
			{ "%from%", from }
		});
		return q;
	}

	template<typename ItBegin, typename ItEnd>
	string where_in(string what, ItBegin begin, ItEnd end)
	{
		stringstream ss;
		ss << what << " in (" << *begin++;
		
		std::for_each(begin, end, [&ss](auto item)
		{
			ss << ", " << item;
		});
		ss << ")";
		return ss.str();
	}

	template<typename ...TTypes>
	class sql_insert
	{
	public:
		//const size_t type_num = sizeof...(TTypes);
		using item_type = std::tuple<TTypes...>;
		using decl_type = repeat<string, sizeof...(TTypes), std::tuple>::type;
	private:
		decl_type _decl;
		string _table;
		list<item_type> _values;
	public:
		template<typename ...TColumns>
		sql_insert(string table, TColumns... columns) : _table(table), _decl(columns)
		{}

		void push_back(item_type& item) { _values.push_back(item); }
		void emplace_back(TTypes... values) { _values.emplace_back(values); }

		operator string() const
		{
			if (_values.empty())
				throw std::runtime_error{ "_values are empty" };

			stringstream ss;
			auto inserter = [&ss](item_type const& item)
			{
				size_t i = 0;
				ss << "(";
				std::apply([&](auto&&... args) 
				{
					([&]()
					{
						if (i > 0)
							ss << ", ";
						ss << args;
						++i;
					} (), ...);
				}, item);
				ss << ")";
			};

			ss << "insert into " << _table;
			{
				ss << "(";
				size_t i = 0;
				std::apply([&](auto&&... args) 
				{
					([&]()
					{
						if (i > 0)
							ss << ", ";
						ss << args;
						++i;
					} (), ...);
				}, _decl);
				ss << ")";
			}				

			ss << " values ";
			auto begin = _values.begin(); auto end = _values.end();
			inserter(*(begin++));
			std::for_each(begin, end, [&ss, &inserter](item_type const& item)
			{
				ss << ", ";
				inserter(item);
			});
			ss << ";";

			return ss.str();
		}
	};
}

#endif //UTILITIES_SQL_HPP