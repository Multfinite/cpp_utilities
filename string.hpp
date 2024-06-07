#ifndef UTILITIES_STRING_HPP
#define UTILITIES_STRING_HPP

#include <string>
#include <memory>
#include <stdexcept>
#include <list>
#include <map>
#include <algorithm>
#include <sstream>

#if STD_FS == 1
    #include <string_view>

    using string_view = std::string_view;
#elif STD_FS == 2
    #include <experimental/string_view>

    using string_view = std::experimental::string_view;
#else
    static_assert (false, "<string_view> not supported");
#endif

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

        operator std::string_view() const { return std::string_view{ value, N }; }
        operator const char*() const { return &value; }
    };
    
    /* https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf */
    template<typename ... Args>
    inline std::string string_format(const std::string& format, Args ... args)
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
        if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
        auto size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }
    
    inline std::string string_tolower(const std::string& str)
    {
        std::string s = str;
        std::transform(
            s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return std::tolower(c); }
        );
        return s;
    }

    /* https://stackoverflow.com/questions/5878775/how-to-find-and-replace-string */
    inline void string_replace_all(
        std::string& s,
        const std::string& toReplace,
        const std::string& replaceWith
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
    
    inline std::string string_replace_all_templates(
        const std::string& s,
        const std::map<std::string, std::string>& replacements
    ) {
        std::string str = s;
        for (auto& kvp : replacements)
        {
            string_replace_all(str, kvp.first, kvp.second);
        }
        return str;
    }
    inline void string_replace_all_templates(
       std::string& s,
       const std::map<std::string, std::string>& replacements
    ) {
        for (auto& kvp : replacements)
        {
            string_replace_all(s, kvp.first, kvp.second);
        }
    }
    
    inline std::list<std::string> string_split(const std::string& str, const std::string& delimiter)
    {
        std::string s = str;
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

    inline std::list<std::string> string_split_ret_s(const std::string& s, const std::string& delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::list<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
        {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));

        if (res.empty())
            res.push_back(s);

        return res;
    }

    template<typename ...T>
    inline std::string string_join(const std::string& separator, const T& ...items)
    {
        std::stringstream s; bool f = true;
        /*
        for(auto& item : {items...})
        {
            if(f) { s << separator; f = true; }
            s << item;
        }
        */
        ([&]
        {
            if(f) { s << separator; f = true; }
            s << items;
        }, ...);
        return s.str();
    }

    inline std::string::size_type string_indexof(const std::string& s, char c)
    {
        auto iter = std::find_if(s.cbegin(), s.cend(), [&c](char chr) { return chr == c; });
        if(iter == s.cend()) throw "not found";
        auto index = (iter - s.cbegin()) /*<- this is not index*/;
        return index;
    }

    inline std::string string_between(const std::string& s, char left, char right)
    {
        auto il = string_indexof(s, left); auto ir = string_indexof(s, right);
        if(ir <= il) throw "ir <= il";
        return s.substr(il, (ir - il) + 1);
    }

    inline std::string string_between(std::string::iterator a, std::string::iterator b)
    {
        std::stringstream ss;
        for(++a; a != b; ++a)
            ss << *a;
        return ss.str();
    }

    inline std::string string_between(std::string::const_iterator a, std::string::const_iterator b)
    {
        std::stringstream ss;
        for(++a; a != b; ++a)
            ss << *a;
        return ss.str();
    }

    inline std::string string_remove_between(const std::string& s, std::string::size_type left, std::string::size_type right)
    {
        if(right <= left) throw "right <= left";
        std::stringstream ss;
        for(auto i = 0; i < s.size(); ++i)
        {
            if(i > left && i < right) continue;
            ss << s[i];
        }
        return ss.str();
    }

    inline std::string string_remove_range(const std::string& s, std::string::size_type left, std::string::size_type right)
    {
        if(right <= left) throw "right <= left";
        std::stringstream ss;
        for(auto i = 0; i < s.size(); ++i)
        {
            if(i >= left && i <= right) continue;
            ss << s[i];
        }
        return ss.str();
    }

    inline std::string string_extract(std::string& s, char left, char right)
    {
        auto il = string_indexof(s, left); auto ir = string_indexof(s, right);
        auto between = s.substr(il, (ir - il) + 1);
        s = string_remove_range(s, il, ir);
        return between;
    }
}

#include <thread>

namespace std
{
    inline std::string to_string(const std::thread::id& tid)
    {
        stringstream ss;
        ss << tid;
        return ss.str();
    }
}

#endif // UTILITIES_STRING_HPP
