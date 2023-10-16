#ifndef UTILITIES_SQL_HPP
#define UTILITIES_SQL_HPP

#include "type_definitions.hpp"
#include "string.hpp"

namespace Utilities::SQL
{
	string select_page(string what, string from, size_t page, size_t perpage)
	{
		string q = "select %what% from %from% limit %limit% offset %offset%;";
		string_replace_all_templates(q,
		{
			{ "%what%", what },
			{ "%from%", from },
			{ "%limit%", std::to_string(perpage) },
			{ "%offset%", std::to_string(page * perpage) }
		});
		return q;
	}

	string count_rows(string from)
	{
		string q = "select count(*) as rows from %from%;";
		string_replace_all_templates(q,
		{
			{ "%from%", from }
		});
		return q;
	}
}

#endif //UTILITIES_SQL_HPP