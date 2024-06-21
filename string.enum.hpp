#ifndef UTILITIES_STRING_ENUM_HPP
#define UTILITIES_STRING_ENUM_HPP

#include <map>

#include "string.hpp"
#include "algorithm.hpp"
#include "exceptions.hpp"

namespace Utilities::Enum
{
    template <typename T>
    std::map<std::string, T> GetMap();

    template<typename T>
    T from_string(const std::string& s)
    {
        try {
            std::map<std::string, T> m = GetMap<T>();
            return m.at(Utilities::string_tolower(s));
        } catch (...) {
            throw construct_error(Exceptions::base_error, "from_string<T = " + std::string(typeid(T).name()) + "> invalid string value v = { " + s + " }");
        }
    }

    template<typename T>
    std::string to_string(T v)
    {
         std::map<std::string, T> m = GetMap<T>();
         auto iter = Utilities::find_if(m, [&v](const typename decltype(m)::value_type& x)
         {
            return x.second == v;
         });
         if(iter != m.end())
             return iter->first;

         throw construct_error(Exceptions::base_error, "to_string<T = " + std::string(typeid(T).name()) + "> invalid value of enum v = { " + std::to_string(static_cast<std::underlying_type_t<T>>(v)) + " }");
    }
}

#if ENABLE_NLOHMANN_JSON == 1
#define JSON_ENUM_TYPE_CONVERSION(enum_type) \
    void to_json(nlohmann::json& j, const enum_type& v) { j = to_string(v); } \
    void from_json(const nlohmann::json& j, enum_type& v) \
    { \
        if(j.is_string()) \
            v = from_string<std::remove_reference_t<decltype(v)>>((std::string) j); \
        else if(j.is_number_integer()) \
            v =  static_cast<std::remove_reference_t<decltype(v)>>((long) j); \
        else if(j.is_number_unsigned()) \
            v = static_cast<std::remove_reference_t<decltype(v)>>((unsigned long) j); \
        else throw construct_error(Exceptions::base_error, "j is not string or int\\uint. ONLY strings and integers are allowed."); \
    } \

#else
#define JSON_ENUM_TYPE_CONVERSION(enum_type)
#endif // ENABLE_NLOHMANN_JSON

#endif // UTILITIES_STRING_ENUM_HPP
