#ifndef UTILITIES_STRING_HPP
#define UTILITIES_STRING_HPP

#include <string>
#include <memory>
#include <stdexcept>
#include <list>
#include <map>
#include <algorithm>

namespace Utilities
{    
    #define nameof(x) #x
    #define stringify(x) #x
    
    template<size_t N>
    struct string_literal 
    {
        constexpr string_literal(const char(&str)[N]) {
            std::copy_n(str, N, value);
        }
    
        char value[N];
    };
    
    /* https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf */
    template<typename ... Args>
    std::string string_format(const std::string& format, Args ... args)
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
        if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
        auto size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }
    
    inline std::string string_tolower(std::string str)
    {
        std::transform(
            str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); }
        );
        return str;
    }

    /* https://stackoverflow.com/questions/5878775/how-to-find-and-replace-string */
    inline void string_replace_all(
        std::string& s,
        std::string toReplace,
        std::string replaceWith
    ) {
        std::string buf;
        std::size_t pos = 0;
        std::size_t prevPos;
    
        // Reserves rough estimate of final size of string.
        buf.reserve(s.size());
    
        while (true) {
            prevPos = pos;
            pos = s.find(toReplace, pos);
            if (pos == std::string::npos)
                break;
            buf.append(s, prevPos, pos - prevPos);
            buf += replaceWith;
            pos += toReplace.size();
        }
    
        buf.append(s, prevPos, s.size() - prevPos);
        s.swap(buf);
    }
    
    inline void string_replace_all_templates(
        std::string& s,
        std::map<std::string, std::string> replacements
    )
    {
        for (auto& kvp : replacements)
        {
            string_replace_all(s, kvp.first, kvp.second);
        }
    }
    
    std::list<std::string> string_split(std::string s, std::string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::list<std::string> res;
    
        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) 
        {
            token           = s.substr (pos_start, pos_end - pos_start);
            pos_start       = pos_end + delim_len;
            res.push_back (token);
        }
    
        res.push_back (s.substr (pos_start));
        return res;
    }
}

#endif // UTILITIES_STRING_HPP