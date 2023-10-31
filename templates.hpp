#ifndef UTILITIES_TEMPLATES_HPP
#define UTILITIES_TEMPLATES_HPP

namespace Utilities
{
    /* https://stackoverflow.com/questions/16853552/how-to-create-a-type-list-for-variadic-templates-that-contains-n-times-the-sam */

    template<typename, typename>
    struct append_to_type_seq { };

    template<typename T, typename... Ts, template<typename...> class TT>
    struct append_to_type_seq<T, TT<Ts...>>
    {
        using type = TT<Ts..., T>;
    };

    template<typename T, unsigned int N, template<typename...> class TT>
    struct repeat
    {
        using type = typename
            append_to_type_seq<
            T,
            typename repeat<T, N - 1, TT>::type
            >::type;
    };

    template<typename T, template<typename...> class TT>
    struct repeat<T, 0, TT>
    {
        using type = TT<>;
    };

    /* https://stackoverflow.com/a/9407521 */

    template<typename T>
    struct has_const_iterator
    {
    private:
        typedef char                      yes;
        typedef struct { char array[2]; } no;

        template<typename C> static yes test(typename C::const_iterator*);
        template<typename C> static no  test(...);
    public:
        static const bool value = sizeof(test<T>(0)) == sizeof(yes);
        typedef T type;
    };

    template <typename T>
    struct is_iterable
    {
        template<typename C> static char(&f(typename std::enable_if<
            std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::begin)),
            typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

        template<typename C> static char(&f(...))[2];

        template<typename C> static char(&g(typename std::enable_if<
            std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::end)),
            typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

        template<typename C> static char(&g(...))[2];

        static bool const beg_value = sizeof(f<T>(0)) == 1;
        static bool const end_value = sizeof(g<T>(0)) == 1;
        static bool const value = beg_value && end_value;
    };

    template<typename T>
    struct is_container : std::integral_constant<bool, has_const_iterator<T>::value && is_iterable<T>::value>
    { };
}

#endif //UTILITIES_TEMPLATES_HPP