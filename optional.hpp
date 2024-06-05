#ifndef UTILITIES_OPTIONAL_HPP
#define UTILITIES_OPTIONAL_HPP

#if STD_FS == 1
    #include <optional>

    template<typename _Tp>
    using optional = std::optional<_Tp>;
#elif STD_FS == 2
    #include <experimental/optional>

    template<typename _Tp>
    using optional = std::experimental::optional<_Tp>;
#else
    static_assert (false, "<optional> not supported");
#endif

#endif // UTILITIES_OPTIONAL_HPP
