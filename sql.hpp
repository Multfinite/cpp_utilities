#ifndef UTILITIES_SQL_HPP
#define UTILITIES_SQL_HPP

#include "type_definitions.hpp"
#include "string.hpp"
#include "templates.hpp"

namespace Utilities::SQL
{
	std::string as_sql(const std::string& v) { return "'" + v + "'"; };
	std::string as_sql(const char* v) { return "'" + std::string(v) + "'"; };
	std::string as_sql(std::string_view v) { return "'" + std::string(v.data()) + "'"; };
	std::string as_sql(uint8_t v) { return std::to_string(v); }
	std::string as_sql(uint16_t v) { return std::to_string(v); }
	std::string as_sql(uint32_t v) { return std::to_string(v); }
	std::string as_sql(uint64_t v) { return std::to_string(v); }
	std::string as_sql(int8_t v) { return std::to_string(v); }
	std::string as_sql(int16_t v) { return std::to_string(v); }
	std::string as_sql(int32_t v) { return std::to_string(v); }
	std::string as_sql(int64_t v) { return std::to_string(v); }
	std::string as_sql(double v) { return std::to_string(v); }
	std::string as_sql(float v) { return std::to_string(v); }

	string select_page(const string& what, const string& from, size_t page, size_t perpage)
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

	string select_page_with_condition(const string& what, const string& from, const string& condition, size_t page, size_t perpage)
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

	string count_rows(const string& from)
	{
		string q = "select count(*) as rows from %from%";
		string_replace_all_templates(q,
		{
			{ "%from%", from }
		});
		return q;
	}

	template<typename ItBegin, typename ItEnd>
	string where_in(const string& what, ItBegin begin, ItEnd end)
	{
		stringstream ss;
		ss << what << " in (";
		if (begin != end)
		{
			ss << as_sql(*begin++);
			std::for_each(begin, end, [&ss](const auto& item)
			{
				ss << ", " << as_sql(item);
			});
		}
		else ss << "select null";
		ss << ")";
		return ss.str();
	}
	template<typename ItBegin, typename ItEnd, typename TExtractor>
	string where_in(const string& what, ItBegin begin, ItEnd end, const TExtractor& extractor)
	{
		stringstream ss;
		ss << what << " in (";
		if(begin != end)
		{
			ss << as_sql(extractor(*begin++));
			std::for_each(begin, end, [&ss, &extractor](const auto& item)
			{
				ss << ", " << as_sql(extractor(item));
			});
		}
		else ss << "select null";
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

	template<typename TDbConn>
	struct SqlInvoker
	{
		void operator()(const TDbConn& db, const std::string& sql) const;
	};

	/*
	template<string_literal name, typename T>
	struct Column
	{
		static inline string_literal Name = name;
	};
	template<string_literal name, typename ...TColumns>
	struct Table
	{
		static inline string_literal Name = name;
	};
	*/

	template<string_literal name, typename ...TArgs>
	struct Procedure
	{
		static inline string_literal Name = name;

		static std::string sql(const TArgs&... args)
		{
			stringstream ss;
			ss << "call " << Name.value << "(";
			size_t i = 0;
			([&]
			{
				if(i++ > 0)
					ss << ", ";
				ss << as_sql(args);
			} (), ...);
			ss << ")";
			return ss.str();
		}

		template<typename TDbConn>
		void operator()(const TDbConn& db, const TArgs&... args) const
		{
			SqlInvoker<TDbConn>{}(db, sql(args));
		}
	};
}

#endif //UTILITIES_SQL_HPP