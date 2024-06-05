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

#endif // UTILITIES_FILESYSTEM_HPP
