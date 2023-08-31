#ifndef HTTP_ENDPOINT_HPP
#define HTTP_ENDPOINT_HPP

#include <exception>
#include <stdexcept>

#include "spdlog.hpp"

#include "type_definitions.hpp"
#include "logging.hpp"
#include "exceptions.hpp"

#include <date/date.h>
#include <libpq-fe.h>

#include "restbed.hpp"

namespace HTTP
{
	using namespace restbed;
	using restbed::Service;
	using namespace Utilities;

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
		virtual int64_t										get_code()		const { return INTERNAL_SERVER_ERROR; }
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief HTTP status code which will be sent back with response.
		* @return [int64_t] HTTP response code.
		*/
		virtual nlohmann::json							get_detail()	const { return nlohmann::json::parse("{}"); }
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief Should be exception thrown.
		* @return [boolean] if true exception will be raised, otherwise false.
		*/
		virtual bool											raise()			const { return false; }
	};
	/*!
	* @author multfinite@gmail.com (multfinite)
	* @brief class which stores information about http response
	*/
	struct HttpResponse
	{
		int64_t														Code			= NOT_IMPLEMENTED;
		string														Content		= "";
		std::multimap<std::string, std::string>		Headers		{};
	};

	/*!
	* @author multfinite@gmail.com (multfinite)
	* @brief This class provides basic HTTP handlers & logging. 
	* @param TService is class which implement all internal lofic of application. It must have implement write_access_log(json information, string sourceAddress, int16_t tag, TData& data). 
	* @param TData is class which stores information to communicate between HttpEndpoint and TService. It will be passed to handler functions.
	*/
	template<class TService, class TData>
	class HttpEndpoint
	{
	private:
		shared_ptr<spdloglib> _spdlog = nullptr;
	public:
		using TServiceConstructor = TService::_constructor;
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief shared_ptr of restbed::Request
		*/
		using HttpRequest						= std::shared_ptr<const restbed::Request>;
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief shared_ptr of restbed::Session
		*/
		using HttpSession						= shared_ptr<Session>;
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief Handler function type for endpoint which not require request content.
		*/
		using EndpointHandler				= std::function<void(const HttpSession session, TService& service, HttpResponse& response, bool& autosend, TData& data)>;
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief Handler function type for endpoint which require request content (with fetching).
		*/
		using FetchedEndpointHandler		= std::function<void(const HttpSession session, string content, TService& service, HttpResponse& response, bool& autosend, TData& data)>;
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief creates content for error response
		* @return [object]
		{
			"message": [string] message,
			"detail": some details
		}
		*/
		static json create_error_response(string msg, optional<nlohmann::json> detail)
		{
			json body{};
			body["message"]	= msg;
			body["detail"]		= detail.value_or(nlohmann::json::parse("{}"));
			return body;
		}
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief add json markers for response
		*/
		static void init_json_content_headers(std::multimap<std::string, std::string>& headers, string& content)
		{
			headers.insert({ "Content-Length", std::to_string(content.size()) });
			headers.insert({ "Content-Type", "application/json" });
		}
	private:
		shared_ptr<TServiceConstructor>	_ctorObj;
		shared_ptr<restbed::Resource>		_resource;
		string											_endpoint;
	public:
		shared_ptr<restbed::Resource> get_resource() const { return _resource; }

		HttpEndpoint(shared_ptr<TServiceConstructor>& ctorObj, string endpoint,
			shared_ptr<spdloglib> spdlog)	:
			//cout(cout),
			_ctorObj(ctorObj),
			_resource(make_shared<Resource>()),
			_endpoint(endpoint),
			_spdlog(spdlog)
		{
			_resource->set_path(_endpoint);
		}
		~HttpEndpoint() = default;

		inline json create_message(
			std::optional<string> message,
			std::optional<string> content,
			HttpResponse& response,
			std::chrono::system_clock::time_point const& receivedTime,
			std::chrono::system_clock::time_point const& handledTime,
			HttpSession const& session,
			TService& service,
			std::optional<json> exception
			)
		{
			auto route			= format_route_block(session);
			auto serv			= service.get_service_info();
			auto req			= format_request_block(content, receivedTime, session);
			auto resp			= format_response_block(response.Content, handledTime, response.Code, response.Headers);
			auto endpoint		= format_endpoint_block(_endpoint, req, resp);

			auto msg			= format_message(message, route, serv, endpoint, exception, {}, receivedTime);
			return msg;
		}

		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief This function creates handler for restbed Resource and provides basic functional for logging, responding and exception handling. This version without content fetching.
		* @param [int16_t] tag - identifier of this procedure
		* @param [string] method 
		* @param [EndpointHandler] handler - user handler
		* @return [HttpEndpoint&] self
		*/
		HttpEndpoint& set_method_handler(int16_t tag, string method, EndpointHandler handler)
		{
			_resource->set_method_handler(method, [this, tag, method, handler]
			(const shared_ptr<Session> session)
			{
				auto receivedTime = std::chrono::system_clock::now();

				auto sendResponse = [this, &session](HttpResponse const& response)
				{
					session->close(response.Code, response.Content, response.Headers);
				};

				TService service = _ctorObj->construct();
				TData data{};
				HttpResponse response{};
				bool autosend = true;

				try
				{					
					handler(session, service, response, autosend, data);
					auto handledTime = std::chrono::system_clock::now();
					if (autosend)
					{
						init_json_content_headers(response.Headers, response.Content);
						sendResponse(response);
					}

					auto msg	= create_message(
						"Success", {}, response,
						receivedTime, handledTime,
						session, service, {}
					);						
					_spdlog->info(msg);
				}
				catch (const service_error& ex)
				{
					auto handledTime = std::chrono::system_clock::now();

					response.Code			= ex.get_code();
					response.Content		= create_error_response(ex.what(), ex.get_detail()).dump();
					init_json_content_headers(response.Headers, response.Content);
					sendResponse(response);

					auto exception	= ex.format_block();
					auto msg			= create_message(
						(string)ex.what(), {}, response,
						receivedTime, handledTime,
						session, service, exception
					);						
					_spdlog->info(msg);

					if (ex.raise())
						throw ex;
				}
				catch (const std::exception& ex)
				{
					auto handledTime = std::chrono::system_clock::now();

					response.Code			= INTERNAL_SERVER_ERROR;
					response.Content		= create_error_response(ex.what(), {}).dump();
					init_json_content_headers(response.Headers, response.Content);
					sendResponse(response);

					auto exception	= format_exception_block(ex);
					auto msg			= create_message(
						(string)ex.what(), {}, response,
						receivedTime, handledTime,
						session, service, exception
					);
					_spdlog->construct_error(msg);

					throw ex;
				}
			});
			return *this;
		}
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief This function creates handler for restbed Resource and provides basic functional for logging, responding and exception handling. This version with content fetching.
		* @param [int16_t] tag - identifier of this procedure
		* @param [string] method
		* @param [FetchedEndpointHandler] handler - user handler
		* @return [HttpEndpoint&] self
		*/
		HttpEndpoint& set_method_handler_with_content(int16_t tag, string method, FetchedEndpointHandler handler)
		{
			_resource->set_method_handler(method, [this, tag, method, handler]
			(const shared_ptr<Session> session)
			{				
				auto receivedTime = std::chrono::system_clock::now();

				const auto& request = session->get_request();
				size_t contentLength = request->get_header("Content-Length", 0);
				session->fetch(contentLength, [this, tag, receivedTime, handler](const HttpSession session, const Bytes& raw)
				{
					auto sendResponse = [this, &session](HttpResponse const& response)
					{
						session->close(response.Code, response.Content, response.Headers);
					};

					string content = restbed::String::to_string(raw);
					TService service = _ctorObj->construct();
					TData data{};
					HttpResponse response{};
					bool autosend = true;

					try
					{
						handler(session, content, service, response, autosend, data);
						auto handledTime = std::chrono::system_clock::now();
						if (autosend)
						{
							init_json_content_headers(response.Headers, response.Content);
							sendResponse(response);
						}
						
						auto msg	= create_message(
							"Success", content, response,
							receivedTime, handledTime,
							session, service, {}
						);						
						_spdlog->info(msg);
					}
					catch (const service_error& ex)
					{
						auto handledTime = std::chrono::system_clock::now();

						response.Code = ex.get_code();
						response.Content = create_error_response(ex.what(), ex.get_detail()).dump();
						init_json_content_headers(response.Headers, response.Content);
						sendResponse(response);

						auto exception	= ex.format_block();
						auto msg			= create_message(
							(string) ex.what(), content, response,
							receivedTime, handledTime,
							session, service, exception
						);						
						_spdlog->info(msg);

						if (ex.raise())
							throw ex;
					}
					catch (const std::exception& ex)
					{
						auto handledTime = std::chrono::system_clock::now();

						response.Code = INTERNAL_SERVER_ERROR;
						response.Content = create_error_response(ex.what(), {}).dump();
						init_json_content_headers(response.Headers, response.Content);
						sendResponse(response);

						auto exception	= format_exception_block(ex);
						auto msg			= create_message(
							(string)ex.what(), content, response,
							receivedTime, handledTime,
							session, service, exception
						);
						_spdlog->construct_error(msg);

						throw ex;
					}
				});
			});
			return *this;
		}
	};
}
#endif // HTTP_ENDPOINT_HPP