#ifndef UTILITIES_OPTIONAL_HPP
#define UTILITIES_OPTIONAL_HPP

#if STD_FS == 1
    #include <optional>

    template<typename _Tp>
    using optional = std::optional<_Tp>;
    using nullopt_t = std::nullopt_t;
    constexpr nullopt_t nullopt { nullopt_t::_Construct::_Token };
#elif STD_FS == 2
    #include <experimental/optional>

    template<typename _Tp>
    using optional = std::experimental::optional<_Tp>;
    using nullopt_t = std::experimental::nullopt_t;
    constexpr nullopt_t nullopt { nullopt_t::_Construct::_Token };
#else
    static_assert (false, "<optional> not supported");
#endif

#endif // UTILITIES_OPTIONAL_HPP
