#ifndef UTILITIES_FILESYSTEM_HPP
#define UTILITIES_FILESYSTEM_HPP

#if STD_FS == 1
    #include <filesystem>
    namespace fs = std::filesystem;
#elif STD_FS == 2
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    static_assert (false, "<filesystem> not supported");
#endif

namespace Utilities
{
    inline fs::path absolute(fs::path const& p) { return p.is_absolute() ? p : (fs::current_path() / p ); }
    inline fs::path absolute(std::string const& p) { return fs::path(p).is_absolute() ? p : (fs::current_path() / p ); }
}

#endif // UTILITIES_FILESYSTEM_HPP
