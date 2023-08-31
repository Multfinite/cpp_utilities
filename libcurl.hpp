#ifndef UTILITIES_LIBCURL_HPP
#define UTILITIES_LIBCURL_HPP

#include <cstdio>
#include <cstdlib>

#include <functional>
#include <exception>

#include <optional>
#include <map>
#include <string>

#include <curl/curl.h>

#include "utilities"

namespace Utilities
{
	class libcurl
	{
	public:
		static constexpr const char UserAgent[] = "libcurl/8.0";
	
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief Error occured into CURL code.
		*/
		struct curl_error : public std::runtime_error
		{
			CURLcode Code;
	
			curl_error(CURLcode code) : std::runtime_error(""), Code(code) { }
			curl_error(CURLcode code, string msg) : std::runtime_error(msg), Code(code) {}
		};
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief CURL initialization failed.
		*/
		struct curl_not_initied_exception : public std::runtime_error 
		{
			curl_not_initied_exception() :std::runtime_error("") {}
		};
		/*!
		* @author multfinite@gmail.com (multfinite)
		* @brief Not enought memory.
		*/
		struct out_of_memory_exception : public std::runtime_error 
		{
			out_of_memory_exception() :std::runtime_error("") {}
		};
	public:
		struct Response
		{
			static string remove_white_space_at_front(string str)
			{
				while (str.size() > 0)
				{
					if (str[0] == ' ')
						str = str.substr(1, str.size() - 1);
					else break;
				}
				return str;
			}
			static list<string> get_lines(string text, string separator)
			{
				list<string> strings{};
	
				while (true)
				{
					size_t entry = text.find(separator);
					if (entry == -1)
						break;
					strings.emplace_back(text.substr(0, entry));
					text = text.substr(entry + separator.size(), text.size() - entry - separator.size());
				}
	
				return strings;
			}
			static std::pair<string, string> parse_header(string line, string separator)
			{
				size_t entry = line.find(separator);
				if (entry == -1)
					throw;
	
				string key = remove_white_space_at_front(line.substr(0, entry));
				string value = remove_white_space_at_front(line.substr(entry + separator.size(), line.size() - entry - separator.size()));
	
				return std::pair{ key, value };
			}
	
			int64_t						Code;
			map<string, string>	Headers{};
			optional<string>		Content;
			Response(int64_t code, string rawResponse, optional<string> content) :
				Code(code),
				Content(content)
			{
				string_replace_all(rawResponse, "\r\n", "\n");
				auto lines = get_lines(rawResponse, "\n");
				for (size_t i = 1; i < lines.size() - 1; i++)
				{
					auto line				= *std::next(lines.begin(), i);
					auto pair				= parse_header(line, ":");
					Headers[pair.first]	= pair.second;
				}
			}
		};
	private:
		static size_t WriteCallback(char* contents, size_t size, size_t nmemb, void* userp)
		{
			((std::string*)userp)->append((char*)contents, size * nmemb);
			return size * nmemb;
		}
		static inline void curl_read_headers(CURL* curl, string& buffer)
		{
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, &buffer);
		}
		static inline void curl_read_content(CURL* curl, string& buffer)
		{
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		}
	public:
		libcurl()
		{
			curl_global_init(CURL_GLOBAL_ALL);
		}
		~libcurl()
		{
			curl_global_cleanup();
		}
	
		inline Response request(string method, string url, std::multimap<string, string> headers, optional<string> body)
		{
			auto curl = curl_easy_init();
			if (!curl)
				throw curl_not_initied_exception{};
	
			string headerBuffer, contentBuffer;
	
			struct curl_slist* curlHeaders = nullptr;
			for (auto& header : headers)
			{
				string headerLine	= string_format("%s: %s", header.first.c_str(), header.second.c_str());
				curlHeaders			= curl_slist_append(curlHeaders, headerLine.c_str());
			}
	
			curl_easy_setopt(		curl, CURLOPT_URL,							url.c_str());
			curl_easy_setopt(		curl, CURLOPT_CUSTOMREQUEST,		method.c_str());
			curl_easy_setopt(		curl, CURLOPT_TIMEOUT,					60);
			curl_easy_setopt(		curl, CURLOPT_HTTPHEADER,				curlHeaders);
			curl_easy_setopt(		curl, CURLOPT_USERAGENT,				UserAgent);
			if (body.has_value())
			{
				curl_easy_setopt(	curl, CURLOPT_POSTFIELDSIZE,			body.value().size());
				curl_easy_setopt(	curl, CURLOPT_POSTFIELDS,				body.value().c_str());
			}
			else
				curl_easy_setopt(	curl, CURLOPT_POSTFIELDSIZE,			0);
	
			curl_read_headers(		curl, headerBuffer);
			curl_read_content(		curl, contentBuffer);
	
			CURLcode result = curl_easy_perform(curl);
			if (result != CURLE_OK)
			{
				string curlErr = curl_easy_strerror(result);
				curl_easy_cleanup(curl);
				throw curl_error{ result, curlErr };
			}
	
			long httpCode = -1;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
	
			Response response{ httpCode, headerBuffer, contentBuffer };
	
			curl_easy_cleanup(curl);
	
			return response;
		}
		inline Response get(string url, std::multimap<string, string> headers, optional<string> body)
		{
			return request("GET", url, headers, body);
		}
		inline Response post(string url, std::multimap<string, string> headers, optional<string> body)
		{
			return request("POST", url, headers, body);
		}
		inline Response put(string url, std::multimap<string, string> headers, optional<string> body)
		{
			return request("PUT", url, headers, body);
		}
		inline Response patch(string url, std::multimap<string, string> headers, optional<string> body)
		{
			return request("PATCH", url, headers, body);
		}
		inline Response del(string url, std::multimap<string, string> headers, optional<string> body)
		{
			return request("DELETE", url, headers, body);
		}
	
		inline string escape_string(string str)
		{
			CURL* curl = curl_easy_init();
			if (!curl)
				throw curl_not_initied_exception{};
	
			char* output = curl_easy_escape(curl, str.c_str(), str.size());
			if (!output)
			{
				curl_free(output);
				curl_easy_cleanup(curl);
				throw new_base_error(Exceptions::base_error, "null result");
			}
			string escaped = output;
			curl_easy_cleanup(curl);
			return escaped;
		}
	};
}

#endif // UTILITIES_LIBCURL_HPP