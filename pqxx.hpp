#ifndef UTILITIES_PQXX_HPP
#define UTILITIES_PQXX_HPP

#include <pqxx/pqxx>

#include "type_definitions.hpp"
#include "string.hpp"
#include "sql.hpp"

#ifndef UTILITIES_SQLPP_NO_JSON
#include "json.hpp"
#endif //UTILITIES_SQLPP_NO_JSON

namespace pqxx
{
	using timestamp = string;
	//using timestamp = std::chrono::system_clock::time_point;
	//
	//template<> std::string const type_name<timestamp> { "timestamp" };
	//template<> struct nullness<timestamp> : pqxx::no_null<timestamp> {};
	//template<> struct string_traits<timestamp>
	//{
	//	static timestamp from_string(std::string_view text) 
	//	{ 
	//		int64_t microseconds = std::stoll(text.data());
	//
	//		return std::chrono::duration_cast<timestamp>(std::chrono::microseconds(microseconds));
	//	}
	//	static zview to_buf(char* begin, char* end, timestamp const& value);
	//	static char* into_buf(char* begin, char* end, timestamp const& value);
	//	static size_t size_buffer(timestamp const& value) noexcept;
	//};
	// std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(value.time_since_epoch()).count())
	//		int64_t t;
	//return ;
}

namespace Utilities::PQXX
{
	using namespace pqxx;
	using namespace Utilities::SQL;

	shared_ptr<connection> connect(
		const string& address, 
		int64_t port, 
		const string& database,
		const string& username,
		const string& userpass,
		const string& clientEncoding = "UTF8"
	) {
		//string connstr = "host=%host% port=%port% dbname=%db% user=%dbuser% password=%dbpass%";
		string connstr = "postgresql://%dbuser%:%dbpass%@%host%:%port%?dbname=%db%&client_encoding=%encoding%";
		string_replace_all_templates(connstr, 
		{
			{ "%host%", address },
			{ "%port%", std::to_string(port) },
			{ "%db%", database },
			{ "%dbuser%", username },
			{ "%dbpass%", userpass },
			{ "%encoding%", clientEncoding }
		});
		
		return shared_ptr<connection>(new connection{ connstr });
	}

	template<typename TTransaction, typename THandler>
	void iterate_items(
		TTransaction& t, THandler handler, 
		const string& what, const string& from, 
		size_t perpage
	) {
		auto rc = t.exec1(count_rows(from));
		auto count = std::get<0>(rc.as<size_t>());

		for (size_t i = 0; i * perpage < count; i++)
		{
			for (auto row : t.exec(select_page(what, from, i, perpage)))
				handler(row); // row.as<TRowParameters...>
		}
	}

	template<typename TTransaction, typename THandler>
	void iterate_items_with_condition(
		TTransaction& t, THandler handler, 
		const string& what, const string& from, const string& condition,
		size_t perpage
	) {
		auto rc = t.exec1(count_rows(from));
		auto count = std::get<0>(rc.as<size_t>());

		for (size_t i = 0; i * perpage < count; i++)
		{
			for (auto row : t.exec(select_page_with_condition(what, from, condition, i, perpage)))
				handler(row); // row.as<TRowParameters...>
		}
	}
#ifndef UTILITIES_SQLPP_NO_JSON

#endif //UTILITIES_SQLPP_NO_JSON
}

#endif //UTILITIES_PQXX_HPP