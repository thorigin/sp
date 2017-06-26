/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_GE_PARSER_TRAITS_HPP
#define	SP_ALGO_GEN_GE_PARSER_TRAITS_HPP

#include "sp/util/tuples.hpp"
#include <vector>
#include <tuple>
#include <bitset>
#include <type_traits>
#include <variant>
#include <optional>


namespace sp { namespace util {
//Forward declared
template<size_t From, size_t To, typename Parent>
struct tuple_view;
}}
/**
 * \brief Defines various type traits
 */
namespace sp { namespace algo { namespace gen { namespace parser { namespace traits {

template<typename T, typename = void>
struct is_iterable : std::false_type {};

template<typename T>
struct is_iterable<
    T,
    std::void_t<decltype(std::begin(std::declval<T>()), std::end(std::declval<T>()))>
> : std::true_type {};

template<typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;



template<typename T, typename = void>
struct is_container : std::false_type {};

template<typename T>
struct is_container<
    T,
    std::void_t<
        typename T::value_type,
        typename T::size_type,
        typename T::allocator_type,
        typename T::iterator,
        typename T::const_iterator,
        decltype(std::declval<T>().size()),
        decltype(std::declval<T>().begin()),
        decltype(std::declval<T>().end()),
        decltype(std::declval<T>().cbegin()),
        decltype(std::declval<T>().cend())
    >
> : std::true_type {};

template<typename T>
constexpr bool is_container_v = is_iterable<T>::value;

//    template<typename T, typename = void>
//    struct is_insertable : std::false_type {};
//
//    template<typename T>
//    struct is_insertable<
//        T,
//        std::void_t<
//            decltype(
//                std::declval<T>().insert()
//            )
//        >
//    > : std::true_type {};
//
//    template<typename T>
//    constexpr bool is_insertable_v = is_insertable<T>::value;

template<typename>
struct is_vector : std::false_type {};

template<typename T, typename ... Template_args>
struct is_vector<std::vector<T, Template_args...>> : std::true_type {};

template<typename T>
constexpr bool is_vector_v = is_vector<T>::value;

template<typename>
struct is_tuple : std::false_type {};

template<typename ... Template_args>
struct is_tuple<std::tuple<Template_args...>> : std::true_type {};

template<size_t From, size_t To, typename Parent>
struct is_tuple<util::tuple_view<From, To, Parent>> : std::true_type {};

template<typename T>
constexpr bool is_tuple_v = is_tuple<T>::value;

template<typename>
struct is_variant : std::false_type {};

template<typename ... Template_args>
struct is_variant<std::variant<Template_args...>> : std::true_type {};

template<typename T>
constexpr bool is_variant_v = is_variant<T>::value;

template<typename>
struct is_optional : std::false_type {};

template<typename T>
struct is_optional<std::optional<T>> : std::true_type {};

template<typename T>
constexpr bool is_optional_v = is_optional<T>::value;

//attribute enable_if shorthands
template<typename T, typename R = void>
using enable_if_empty_attribute_t = std::enable_if_t<std::is_same_v<T, empty_attribute>, R>;

template<typename T, typename R = void>
using enable_if_vector_t = std::enable_if_t<traits::is_vector_v<T>, R>;

template<typename T, typename R = void>
using enable_if_tuple_t = std::enable_if_t<traits::is_tuple_v<T>, R>;

template<typename T, typename R = void>
using enable_if_variant_t = std::enable_if_t<traits::is_variant_v<T>, R>;

template<typename T, typename R = void>
using enable_if_not_tuple_t = std::enable_if_t<!traits::is_tuple_v<T>, R>;

template<typename T, typename R = void>
using enable_if_optional_t = std::enable_if_t<traits::is_optional_v<T>, R>;

template<typename T, typename R = void>
using enable_if_container_t = std::enable_if_t<traits::is_container_v<T>, R>;

/**
 * \brief Checks if an element of a tuple is itself a tuple as well
 * \remark SFINAE error translates to false
 */
template<typename T, size_t Index = 0u, typename = void>
struct is_tuple_element : std::false_type {};

template<typename T, size_t Index>
struct is_tuple_element<T, Index, enable_if_tuple_t<std::tuple_element_t<0, T>>> : std::true_type {};

template<typename T, size_t Index = 0u>
constexpr bool is_tuple_element_v = is_tuple_element<T, Index>::value;

namespace detail {
    \
    template<typename Tuple, typename Indexes>
    struct tuple_to_variant_helper;

    template<typename Tuple, size_t ... Ns>
    struct tuple_to_variant_helper<Tuple, std::index_sequence<Ns...>>  {
        using type = std::variant<
            std::tuple_element_t<Ns, Tuple>...
        >;
    };
}

template<typename Tuple>
struct tuple_to_variant {
    using type = typename detail::tuple_to_variant_helper<
        Tuple,
        std::make_index_sequence<std::tuple_size_v<Tuple>>
    >::type;
};

template<typename Tuple>
using tuple_to_variant_t = typename tuple_to_variant<Tuple>::type;


}}}}}

#endif	/* SP_ALGO_GEN_GE_PARSER_TRAITS_HPP */

