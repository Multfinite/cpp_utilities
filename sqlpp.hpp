#ifndef UTILITIES_SQLPP_HPP
#define UTILITIES_SQLPP_HPP

#include "type_definitions.hpp"
#include "exceptions.rest.hpp"

#include <sqlpp11/postgresql/connection.h>

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

#include <sqlpp11/insert.h>
#include <sqlpp11/select.h>
#include <sqlpp11/update.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/result.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/custom_query.h>

#include <optional>

#ifndef UTILITIES_SQLPP_NO_JSON
#include "json.hpp"
#endif //UTILITIES_SQLPP_NO_JSON

#define sqlpp_field_spec(table, column, sqlpp_type, required) sqlpp::field_spec_t<table##_::column::_alias_t, sqlpp_type, required>

#define declare_column(type_name, name, data_type, ...) \
struct type_name \
{ \
	struct _alias_t \
	{ \
		static constexpr const char _literal[] = name; \
		using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>; \
		template<typename T> \
		struct _member_t \
		{ \
			T type_name; \
			T& operator()() { return type_name; } \
			const T& operator()() const { return type_name; } \
		}; \
	}; \
	using _traits = sqlpp::make_traits<data_type, ##__VA_ARGS__>; \
} \


#define declare_table(type_name, name, ...) \
struct type_name : sqlpp::table_t<type_name, __VA_ARGS__> \
{ \
	struct _alias_t \
	{ \
		static constexpr const char _literal[] = name; \
		using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>; \
		template<typename T> \
		struct _member_t \
		{ \
			T type_name; \
			T& operator()() { return type_name; } \
			const T& operator()() const { return type_name; } \
		}; \
	}; \
} \

#define declare_table_body(type_name, name) \
struct _alias_t \
{ \
	static constexpr const char _literal[] = name; \
	using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>; \
	template<typename T> \
	struct _member_t \
	{ \
		T type_name; \
		T& operator()() { return type_name; } \
		const T& operator()() const { return type_name; } \
	}; \
} \

// Simplify pagination checks.
#define PAGINATION_CHECK(pageIndex, pageItems) \
	bool paginated = (pageIndex.has_value() && pageItems.has_value()); \
	if (!( \
		paginated || (!pageIndex.has_value() && !pageItems.has_value()) \
		)) \
		throw construct_error(invalid_argument_error, "pageIndex and pageItems must be defined at pair or not defined at all."); \
	if (pageItems.has_value() && pageItems.value() == 0) \
		throw construct_error(invalid_argument_error, "pageItems can not be zero (0) !"); \

namespace Utilities::SQLPP
{    
	using DatabaseConfig = sqlpp::postgresql::connection_config;
	using DatabaseConnection = sqlpp::postgresql::connection;

	using std::string;
	using std::stringstream;
	using std::optional;
	using std::list;

	using identifier_type = std::string;
	using sqlpp_identifier_type = sqlpp::text;
	inline sqlpp_identifier_type::_cpp_value_type internal_to_sqlpp_identifier(identifier_type identifier)
	{
		return identifier;
	}
	inline identifier_type sqlpp_to_internal_identifier(sqlpp_identifier_type::_cpp_value_type identifier)
	{
		return identifier;
	}

	template<typename object_type>
	inline identifier_type generate_identifier()
	{
		using namespace uuids;

		std::random_device rd;
		auto seed_data = std::array<int, std::mt19937::state_size> {};
		std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
		std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
		std::mt19937 generator(seq);
		uuids::uuid_random_generator gen{ generator };

		uuid const id = gen();
		assert(!id.is_nil());
		assert(id.as_bytes().size() == 16);
		assert(id.version() == uuids::uuid_version::random_number_based);
		assert(id.variant() == uuids::uuid_variant::rfc);

		return uuids::to_string(id);
	}
	template<typename object_type>
	inline identifier_type generate_identifier(object_type& t) { return generate_identifier<object_type>(); }

	/*!
	* @brief sqlpp result type to optional<T>.
	*/
	template<typename TResultType>
	inline auto get_value(TResultType r)
	{
		using data_type = TResultType::_traits::_value_type::_cpp_value_type;
		std::optional<data_type> ret{};
		if (r.is_null())
			ret.reset();
		else
			ret = r.value();
		return ret;
	}

	/*!
	* @brief Convert sqlpp_type to string SQL representation.
	*/
	template<typename sqlpp_type>
	string get_schema_type_name() { throw "NOT IMPLEMENTED"; }
	template<> string get_schema_type_name<sqlpp::text>() { return "text"; }
	template<> string get_schema_type_name<sqlpp::integer>() { return "int"; }
	template<> string get_schema_type_name<sqlpp::time_point>() { return "timestamp"; }

	/*!
	* @brief Convert table columns to sequence of it's names.
	*/
	template<typename sqlpp_table_type>
	vector<string> all_of_as_list(sqlpp_table_type& t);
	#define column_nameof(table, column) table##_::column::_alias_t::_literal
	#define table_nameof(table) table::_alias_t::_literal

#ifndef UTILITIES_SQLPP_NO_JSON
	using nlohmann::json;

	/*!
	* @breif Set json object field wi
	*/
	template<typename result_field>
	void field_to_json(result_field& item, json& j)
	{
		sqlpp::for_each_field(item, [&j](const auto& field)
		{
			string name = sqlpp::get_sql_name(field);
			auto value = field.value();
			j[name] = Utilities::JSON::json_compatible(value);
		});
	}

	/*!
	* @brief Perform insertion of given items to table
	* @param [db] The sqlpp database connection
	* @param [t] sqlpp table type which is insertion target
	* @param [items] container of json's items
	* @param [returning] column to return of inserted items, default to "id"
	*/
	template<typename sqlpp_table_type, typename container_type>
	auto insert_json_into(DatabaseConnection& db, sqlpp_table_type& t, container_type& items, string returning = "id")
	{
		if (items.empty())
			construct_error_no_msg(Exceptions::REST::invalid_argument_error);

		std::string query = "insert into %table% %columns% values %values% returning %returning%;";
		// table name
		Utilities::string_replace_all_templates(query, {
			{ "%table%", sqlpp_table_type::_alias_t::_literal },
			{ "%returning%", returning } 
		});

		// column insert definition
		auto columns = all_of_as_list(t);
		for (auto& col : columns)
			Utilities::string_replace_all(query, "%columns%", "%b%" + col + "%e%");
		Utilities::string_replace_all(query, "%e%%b%", ",");
		Utilities::string_replace_all_templates(query, { { "%b%", "" }, { "%e%", "" } });

		// items
		stringstream ss;
		for (json& item : items)
		{
			ss << "%b%(";
			for (auto& column : columns)
			{
				ss << "%b%" << (item.contains(column) ? item[column] : "default") << "%e%";
			}
			ss << ")%e%";
		}
		Utilities::string_replace_all(query, "%values%", ss.str());
		Utilities::string_replace_all(query, "%e%%b%", ",");
		Utilities::string_replace_all_templates(query, { { "%b%", "" }, { "%e%", "" } });

		auto q = sqlpp::custom_query(sqlpp::verbatim(query))
			.with_result_type_of(sqlpp::select(sqlpp::value(0).as(sqlpp::alias::a)));
		return db(q);
	}

	/*!
	* @brief Perform insertion of given items to table
	* @param [db] The sqlpp database connection
	* @param [t] sqlpp table type which is insertion target
	* @param [items] container of json's items [json(json const& rawItem)]
	* @param [itemHandler] do something with json item before insertion
	* @param [returning] column to return of inserted items, default to "id"
	*/
	template<typename sqlpp_table_type, typename container_type, typename callable_type >
	auto insert_json_into_with_callable(DatabaseConnection& db, sqlpp_table_type& t, container_type& items, callable_type itemHandler, string returning = "id")
	{
		if (items.empty())
			construct_error_no_msg(Exceptions::REST::invalid_argument_error);

		// table name
		std::string query = "insert into %table% %columns% values %values% returning %returning%;";
		Utilities::string_replace_all_templates(query, {
			{ "%table%", sqlpp_table_type::_alias_t::_literal },
			{ "%returning%", returning } 
		});

		// column insert definition
		auto columns = all_of_as_list(t);
		for (auto& col : columns)
			Utilities::string_replace_all(query, "%columns%", "%b%" + col + "%e%");
		Utilities::string_replace_all(query, "%e%%b%", ",");
		Utilities::string_replace_all_templates(query, { { "%b%", "" }, { "%e%", "" } });

		// items
		stringstream ss;
		for (json& rawItem : items)
		{
			ss << "%b%(";
			auto item = itemHandler(rawItem);
			for (auto& column : columns)
			{
				ss << "%b%" << (item.contains(column) ? item[column] : "default") << "%e%";
			}
			ss << ")%e%";
		}
		Utilities::string_replace_all(query, "%values%", ss.str());
		Utilities::string_replace_all(query, "%e%%b%", ",");
		Utilities::string_replace_all_templates(query, { { "%b%", "" }, { "%e%", "" } });

		auto q = sqlpp::custom_query(sqlpp::verbatim(query))
			.with_result_type_of(sqlpp::select(sqlpp::value(0).as(sqlpp::alias::a)));
		return db(q);
	}

	/*!
	* @brief Update table row by given json object representation
	* @param [db] The sqlpp database connection
	* @param [t] sqlpp table type which is insertion target
	* @param [j] json where each key is column 
	* @param[where] selector of items to update. for example it can be: "t.identifier = \"xxxx-...\""
	*/
	template<typename sqlpp_table_type>
	auto update_json(DatabaseConnection& db, sqlpp_table_type& t, nlohmann::json& j, std::optional<std::string> where = std::nullopt)
	{
		std::string query = "update %table% as t set %values%%where%;";
		// table name
		Utilities::string_replace_all_templates(query, {
			{ "%table%", sqlpp_table_type::_alias_t::_literal }
		});
		// SET t.c = value
		stringstream ss;
		for (auto& kvp : j.items())
		{
			ss << "%b%"
				<< "t." << kvp.key() << " = "
				<< kvp.value()
				<< "%e%";
		}
		std::string values = ss.str();
		Utilities::string_replace_all(values, "%e%%b%", ",");
		Utilities::string_replace_all_templates(values, { { "%b%", "" }, { "%e%", "" } });
		Utilities::string_replace_all(query, "%values%", values);
		Utilities::string_replace_all(query, "%where%", where ? (" " + *where) : "");

		auto q = sqlpp::custom_query(sqlpp::verbatim(query))
			.with_result_type_of(sqlpp::update(sqlpp::value(0).as(sqlpp::alias::a)));
		return db(q);
	}

// if page and perpage parameters is correct then select page otherwise select all
#define SQLPP_CONDITIONAL_SELECT(db, o, columns, condition, items, page, perpage) \
{ \
	if (page.has_value() && perpage.has_value()) \
	{ \
		auto query = sqlpp::select(columns).limit(perpage.value()).offset(page.value() * perpage.value()).from(o).where(condition); \
		auto rows = db(query); \
		for (auto& item : rows) \
		{ \
			json& j = items.emplace_back(); \
			field_to_json(item, j); \
		} \
	} \
	else \
	{ \
		auto query = sqlpp::select(columns).from(o).where(condition); \
		auto rows =db(query); \
		for (auto& item : rows) \
		{ \
			json& j = items.emplace_back(); \
			field_to_json(item, j); \
		} \
	} \
} \

#endif // UTILITIES_SQLPP_NO_JSON

	template<typename sqlpp_table, typename sqlpp_column_tuple_t,  typename container_type>
	int64_t select(
		DatabaseConnection& db,
		sqlpp_table& o,
		sqlpp_column_tuple_t columns,
		container_type& items,
		optional<size_t> page = {},
		optional<size_t> perpage = {}
	) {
		if (page.has_value() && perpage.has_value())
		{
			auto query = sqlpp::select(columns)
				.limit(perpage.value()).offset(page.value() * perpage.value())
				.from(o).unconditionally();

			auto rows = db(query);
			for (auto& item : rows)
			{
				json& j = items.emplace_back();
				field_to_json(item, j);
			}
			return rows.size();
		}
		else
		{
			auto query = sqlpp::select(columns).from(o).unconditionally();

			auto rows = db(query);
			for (auto& item : rows)
			{
				json& j = items.emplace_back();
				field_to_json(item, j);
			}
			return rows.size();
		}
	}

	template<typename sqlpp_table_type, typename sqlpp_column_type, typename container_type>
	inline auto select_in(DatabaseConnection& db, sqlpp_table_type& t, sqlpp_column_type& tc, container_type& values)
	{
		auto q = sqlpp::select(all_of(t)).from(t).where(tc.in(sqlpp::value_list(values)));
		auto r = db(q);
		return r;
	}

	template<typename sqlpp_table_type, typename sqlpp_column_type, typename container_type>
	inline bool is_exists(DatabaseConnection& db, sqlpp_table_type& t, sqlpp_column_type& tc, container_type& values)
	{
		auto q = sqlpp::select(all_of(t)).from(t).where(tc.in(sqlpp::value_list(values)));
		auto r = db(q);
		return !r.empty();
	}

	template<typename sqlpp_table_type, typename sqlpp_column_type, typename container_type>
	inline bool is_unique(DatabaseConnection& db, sqlpp_table_type& t, sqlpp_column_type& tc, container_type& values)
	{
		auto q = sqlpp::select(all_of(t)).from(t).where(tc.in(sqlpp::value_list(values)));
		auto r = db(q);
		return r.empty();
	}

	template<typename sqlpp_table_type, typename sqlpp_column_type, typename container_type>
	inline void check_unique(DatabaseConnection& db, sqlpp_table_type& t, sqlpp_column_type& tc, container_type& values)
	{
		if (!Utilities::SQLPP::is_unique(db, t, tc, values))
			throw construct_error(Exceptions::REST::object_already_exist_error, (std::string)"at least one value in <" + sqlpp_table_type::_alias_t::_literal + ":" + sqlpp_column_type::_alias_t::_literal + ">  already exists ");
	}

#define sqlpp_check_exists(db, t_type, t, tc, values) \
	if (Utilities::SQLPP::is_exists(db, t, tc, values)) \
		throw construct_error_args(Exceptions::REST::object_not_found_error, (std::string)"object not found: " + t_type::_alias_t::_literal + ":" + values.front(), values.front()); \

#define throw_object_not_found(t_type, ident) throw construct_error_args(Exceptions::REST::object_not_found_error, (std::string)"object not found: " + t_type::_alias_t::_literal + ":" + ident, ident);
#define throw_object_already_exist(t_type, ident) throw construct_error_args(Exceptions::REST::object_already_exist_error, (std::string)"object already exists: " + t_type::_alias_t::_literal + ":" + ident, ident);

}

#endif // UTILITIES_SQLPP_HPP