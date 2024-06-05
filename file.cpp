#if STD_FS == 1
#include "file.hpp"

namespace Utilities
{
    void get_files(fs::path path, std::list<fs::path>& files, bool recursive)
    {
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (entry.is_regular_file())
                files.push_back(entry.path());
            else if (entry.is_directory() && recursive)
                get_files(entry.path(), files, recursive);
        }
    }
    void get_files(fs::path path, std::list<fs::path>& files, filename_predicate predicate, bool recursive)
    {
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (!predicate(entry))
                continue;
            if (entry.is_regular_file())
                files.push_back(entry.path());
            else if (entry.is_directory() && recursive)
                get_files(entry.path(), files, predicate, recursive);
        }
    }
}
#endif
