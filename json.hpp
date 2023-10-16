#ifndef UTILITIES_JSON_HPP
#define UTILITIES_JSON_HPP

#include <string>
#include <memory>

#include <nlohmann/json.hpp>

#include "string.hpp"
#include "exceptions.hpp"

namespace Utilities
{    
    inline std::string get_json_type_name(nlohmann::json::value_t type)
    {
        switch (type)
        {
            case nlohmann::json::value_t::null:
                return "null";
            case nlohmann::json::value_t::object:
                return "object";
            case nlohmann::json::value_t::array:
                return "array";
            case nlohmann::json::value_t::string:
                return "string";
            case nlohmann::json::value_t::boolean:
                return "boolean";
            case nlohmann::json::value_t::binary:
                return "binary";
            case nlohmann::json::value_t::discarded:
                return "discarded";
            case nlohmann::json::value_t::number_integer:
            case nlohmann::json::value_t::number_unsigned:
            case nlohmann::json::value_t::number_float:
            default:
                return "number";
        }
        throw;
    }
    
    inline void validate_json_object(
        std::string function, std::string file, int line,
        nlohmann::json const& item, 
        std::optional<std::map<std::string, nlohmann::json::value_t>> requiredKeys)
    {
        auto prefix = function + " [" + file + ":" + std::to_string(line) + "]";
        if (!item.is_object())
            throw std::invalid_argument { prefix + " json is not object"};
    
        if (!requiredKeys.has_value())
            return;
    
        for (auto& kvp : requiredKeys.value())
        {
            if (!item.contains(kvp.first))
                throw std::invalid_argument{ prefix + " missing key { " + kvp.first + ":[" + get_json_type_name(kvp.second) + "] }"};
            auto prop = item[kvp.first];
            if(prop.type() != kvp.second)
                throw std::invalid_argument{ prefix + " invalid key type { " + kvp.first + ":[" + get_json_type_name(kvp.second) + "] }" };
        }
    }

    inline nlohmann::json get_setting(nlohmann::json settings, std::string path)
    {
        auto splitten               = string_split(path, ".");
        nlohmann::json& j      = settings;
        std::stringstream verifiedPath{};
        for (size_t i = 0; i < splitten.size(); i++)
        {
            auto key = splitten.begin();
            std::advance(key, i);
    
            if (!j.contains(*key))
                throw construct_error(Exceptions::base_error, verifiedPath.str() + " OF " + path);
            j = j[*key];
    
            if (i > 0)
                verifiedPath << ".";
            verifiedPath << *key;
        }
        return j;
    }
    
    template<typename T>
    T json_value_or(nlohmann::json& j, std::string key, T defaultValue)
    {
        if (j.contains(key))
            return (T)j[key];
        return defaultValue;
    }

    template<typename T>
    inline auto json_compatible(T& value) { return value; }

    template<>
    inline auto json_compatible<std::chrono::system_clock::time_point>(std::chrono::system_clock::time_point& value)
    { 
        return std::chrono::duration_cast<std::chrono::microseconds>(value.time_since_epoch()).count(); 
    }

    template<>
    inline auto json_compatible<  std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>>(std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>& value)
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(value.time_since_epoch()).count();
    }  
}

#define validate_json_object_(item, requiredKeys) Utilities::validate_json_object(__FUNCTION__, __FILE__, __LINE__, item, requiredKeys)

#endif // UTILITIES_JSON_HPP