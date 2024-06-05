#ifndef UTILITIES_EXCEPTIONS_REST_HPP
#define UTILITIES_EXCEPTIONS_REST_HPP

#include <string>

#include "exceptions.hpp"

namespace Exceptions::REST
{
	using std::string;
	/*!
	* @author multfinite@gmail.com (multfinite)
	* @brief An error occured by service code. This error class provides some functions to provide information to HttpEndpoint about responding.
	*/
	struct service_error : public Exceptions::base_error
	{
		service_error(string function, string file, int line) : Exceptions::base_error("An unknown service error", function, file, line) {}
		service_error(string msg, string function, string file, int line) : Exceptions::base_error(msg, function, file, line) { }
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief HTTP status code which will be sent back with response.
		* @return [int64_t] HTTP response code.
		*/
		virtual int64_t get_code() const { return 500; /*INTERNAL_SERVER_ERROR;*/ }
#if ENABLE_NLOHMANN_JSON == 1
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief HTTP status code which will be sent back with response.
		* @return [int64_t] HTTP response code.
		*/
		virtual nlohmann::json get_detail() const { return nlohmann::json::parse("{}"); }
#endif //ENABLE_NLOHMANN_JSON
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief Should be exception thrown.
		* @return [boolean] if true exception will be raised, otherwise false.
		*/
		virtual bool raise() const { return false; }
	};

	/*!
	* @author multfinite
	* @brief
	*/
	struct invalid_argument_error : public service_error
	{
		string Key;

		invalid_argument_error(string function, string file, int line) : service_error(function, file, line), Key("") {}
		invalid_argument_error(string msg, string function, string file, int line) : service_error(msg, function, file, line), Key("") {}
		invalid_argument_error(string function, string file, int line, string key) : service_error(function, file, line), Key(key) {}
		invalid_argument_error(string msg, string function, string file, int line, string key) : service_error(msg, function, file, line), Key(key) {}
		int64_t get_code() const override { return 406 /*NOT_ACCEPTABLE*/; }
	};
	/*!
	* @author multfinite
	* @brief
	*/
	struct missing_argument_error : public service_error
	{
		string Key;

		missing_argument_error(string function, string file, int line, string key) : service_error(function, file, line), Key(key) {}
		missing_argument_error(string msg, string function, string file, int line, string key) : service_error(msg, function, file, line), Key(key) {}
		int64_t get_code() const override { return 406 /*NOT_ACCEPTABLE*/; }
	};
	/*!
	* @author multfinite
	* @brief
	*/
	struct missing_header_error : public service_error
	{
		string Header;

		missing_header_error(string function, string file, int line, string header) : service_error(function, file, line), Header(header) {}
		missing_header_error(string msg, string function, string file, int line, string header) : service_error(msg, function, file, line), Header(header) {}
		int64_t get_code() const override { return 406 /*NOT_ACCEPTABLE*/; }
	};
	/*!
	* @author multfinite
	* @brief
	*/
	struct object_already_exist_error : public service_error
	{
		string Identifier;

		object_already_exist_error(string function, string file, int line, string identifier) : service_error(function, file, line), Identifier(identifier) {}
		object_already_exist_error(string msg, string function, string file, int line, string identifier) : service_error(msg, function, file, line), Identifier(identifier) {}
		object_already_exist_error(string function, string file, int line) : service_error(function, file, line), Identifier("") {}
		object_already_exist_error(string msg, string function, string file, int line) : service_error(msg, function, file, line), Identifier("") {}
		int64_t get_code() const override { return 406 /*NOT_ACCEPTABLE*/; }
	};
	/*!
	* @author multfinite
	* @brief
	*/
	struct object_not_found_error : public service_error
	{
		string Identifier;

		object_not_found_error(string function, string file, int line, string identifier) : service_error(function, file, line), Identifier(identifier) {}
		object_not_found_error(string msg, string function, string file, int line, string identifier) : service_error(msg, function, file, line), Identifier(identifier) {}
		object_not_found_error(string function, string file, int line) : service_error(function, file, line), Identifier("") {}
		object_not_found_error(string msg, string function, string file, int line) : service_error(msg, function, file, line), Identifier("") {}
		int64_t get_code() const override { return 406 /*NOT_ACCEPTABLE*/; }
	};
	/*!
	* @author multfinite
	* @brief
	*/
	struct invalid_identifier_error : public service_error
	{
		string Identifier;

		invalid_identifier_error(string function, string file, int line, string identifier) : service_error(function, file, line), Identifier(identifier) {}
		invalid_identifier_error(string msg, string function, string file, int line, string identifier) : service_error(msg, function, file, line), Identifier(identifier) {}
		int64_t get_code() const override { return 406 /*NOT_ACCEPTABLE*/; }
	};
	/*!
	* @author multfinite
	* @brief
	*/
	struct database_error : public service_error
	{
		database_error(string function, string file, int line) : service_error(function, file, line) {}
		database_error(string msg, string function, string file, int line) : service_error(msg, function, file, line) {}
		int64_t get_code() const override { return 500; /*INTERNAL_SERVER_ERROR;*/ }
	};
}

#endif //UTILITIES_EXCEPTIONS_REST_HPP
