#ifndef UTILITIES_ENVINRONMENT_HPP
#define UTILITIES_ENVINRONMENT_HPP

#include <string>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace Utilities
{     
    inline bool has_argument(int argc, char** argv, std::string key)
    {
        for (auto i = 0; i < argc; i++)
        {
            std::string arg = argv[i];
            if (arg == key)
                return true;
        }
        return false;
    }
    
    inline std::vector<std::string> wrap_args(int argc, char** argv)
    {
        std::vector<std::string> items; items.resize(argc);
        for (int i = 0; i < argc; i++)
            items[i] = argv[i];
        return items;
    }
    
    inline std::optional<std::string> get_envinronment_variable(const char* varname)
    {
        char* v = std::getenv(varname);
        std::optional<std::string> opt; opt.reset();
        if (v != nullptr)
            opt = v;
        return opt;
    }
}

#endif // UTILITIES_ENVINRONMENT_HPP