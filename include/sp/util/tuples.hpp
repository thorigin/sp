/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_UTIL_TUPLES_HPP
#define	SP_UTIL_TUPLES_HPP

#include "sp/config.hpp"
#include <type_traits>
#include <utility>
#include <tuple>

/**
 * \file tuples.hpp
 *
 * Tuple and Tuple type manipulation utilities
 */
SP_UTIL_NAMESPACE_BEGIN

namespace detail {

    template <std::size_t Offset, typename Tuple, std::size_t... Ns>
    constexpr auto tuple_slice_impl_helper(Tuple&& t, std::index_sequence<Ns...>) {
        return std::forward_as_tuple(
            std::get<Ns + Offset>(std::forward<Tuple>(t))...
        );
    }

    template <std::size_t Offset, typename Tuple, std::size_t... Ns>
    constexpr auto tuple_slice_impl_helper(Tuple& t, std::index_sequence<Ns...>) {
        return std::make_tuple(
            std::ref(std::get<Ns + Offset>(t))...
        );
    }

    template <std::size_t From, std::size_t To, typename Tuple>
    constexpr auto tuple_slice_impl(Tuple& t) {
        static_assert(From <= To, "From must be less or equal to To");
        static_assert(std::tuple_size_v<std::decay_t<Tuple>> >= To, "Slice index must be in range of tuple");
        return tuple_slice_impl_helper<From>(
            t,
            std::make_index_sequence<To - From>{}
        );
    }

    template <std::size_t From, std::size_t To, typename Tuple>
    constexpr auto tuple_slice_impl(Tuple&& t) {
        static_assert(From <= To, "From must be less or equal to To");
        static_assert(std::tuple_size_v<std::decay_t<Tuple>> >= To, "Slice index must be in range of tuple");
        return tuple_slice_impl_helper<From>(
            std::forward<Tuple>(t),
            std::make_index_sequence<To - From>{}
        );
    }

    template<typename>
    struct tuple_pop_front {};

    template<typename First, typename ... Others>
    struct tuple_pop_front<std::tuple<First, Others...>> {
        using type = std::tuple<Others...>;
    };

    template<typename, typename>
    struct tuple_pop_end_impl_helper {};

    template<typename ... Ts, size_t ... Ns>
    struct tuple_pop_end_impl_helper<std::tuple<Ts...>, std::index_sequence<Ns...>> {
        using tuple_type = std::tuple<Ts...>;
        using type = std::tuple<
            std::tuple_element_t<
                Ns,
                tuple_type
            >...
        >;
    };

    template<typename Tuple>
    struct tuple_pop_end_impl {
        using type = typename tuple_pop_end_impl_helper<
            Tuple,
            std::make_index_sequence<std::tuple_size_v<Tuple>-1u>
        >::type;
    };

    template<typename, typename>
    struct tuple_push_back_impl {};

    template<typename ... Types, typename Element>
    struct tuple_push_back_impl<std::tuple<Types...>, Element> {
        using type = std::tuple<Types..., Element>;
    };

    template<typename, typename>
    struct tuple_push_front_impl {};

    template<typename ... Rest, typename Element>
    struct tuple_push_front_impl<std::tuple<Rest...>, Element> {
        using type = std::tuple<Element, Rest...>;
    };

}

/**
 * \brief Slice a tuple
 * \tparam From (inclusive)
 * \tparam To (inclusive)
 */
template <std::size_t From, std::size_t To, typename Tuple>
constexpr auto tuple_slice(Tuple& t) {
    return detail::tuple_slice_impl<From, To>(t);
}

/**
 * \brief Slice a tuple (perfect-forwarding)
 * \tparam From (inclusive)
 * \tparam To (inclusive)
 */
template <std::size_t From, std::size_t To, typename Tuple>
constexpr auto tuple_slice(Tuple&& t) {
    return detail::tuple_slice_impl<From, To>(std::forward<Tuple>(t));
}

/**
 * \brief helper template variable for constructing tuple slices (rvalue)
 *
 * \tparam From (inclusive)
 * \tparam To (inclusive)
 */
template<size_t From, size_t To, typename Tuple>
using tuple_slice_rvalue_t = decltype(tuple_slice<From, To>(std::declval<Tuple>()));

/**
 * \brief helper template variable for constructing tuple slices (default lvalue)
 *
 * \tparam From (inclusive)
 * \tparam To (inclusive)
 */
template<size_t From, size_t To, typename Tuple>
using tuple_slice_t = decltype(tuple_slice<From, To>(std::declval<Tuple&>()));

/**
 * \brief helper template variable for constructing tuple slices (lvalue)
 *
 * \tparam From (inclusive)
 * \tparam To (inclusive)
 */
template<size_t From, size_t To, typename Tuple>
using tuple_slice_lvalue_t = decltype(tuple_slice<From, To>(std::declval<Tuple&>()));

/**
 * \brief Get the first element type of a tuple
 */
template<typename Tuple>
using tuple_head_t = std::tuple_element_t<0, Tuple>;

/**
 * \brief Get the last element of a tuple
 */
template<typename Tuple>
using tuple_tail_t = std::tuple_element_t<std::tuple_size_v<Tuple>-1u, Tuple>;

/**
 * \brief Remove the first element of a tuple
 */
template<typename Tuple>
using tuple_pop_front_t = typename detail::tuple_pop_front<Tuple>::type;

/**
 * \brief Removes the first element of a tuple
 */
template<typename Tuple>
using tuple_pop_t = tuple_pop_front_t<Tuple>;

/**
 * \brief Removes the last element from the end
 */
template<typename Tuple>
using tuple_pop_end_t = typename detail::tuple_pop_end_impl<Tuple>::type;

/**
 * \brief Push an element to the end of a tuple
 */
template<typename Tuple, typename T>
using tuple_push_back_t = typename detail::tuple_push_back_impl<Tuple, T>::type;

/**
 * \brief Push an element to the front of a tuple
 */
template<typename Tuple, typename T>
using tuple_push_back_t = typename detail::tuple_push_back_impl<Tuple, T>::type;

/**
 * \brief Push an element to the front of a tuple
 */
template<typename Tuple, typename T>
using tuple_push_t = tuple_push_back_t<Tuple, T>;

/**
 * \brief Push an element to the front of a tuple
 */
template<typename Tuple, typename T>
using tuple_push_front_t = typename detail::tuple_push_front_impl<Tuple, T>::type;

/**
 * \brief Tuple View
 *
 * A simple wrapper around tuples that supports various
 * slicing operations (reductively) while keeping a reference
 * to its original form
 *
 * \remark Use std::get for retrieval
 *
 * \tparam From (inclusive)
 * \tparam To (inclusive)
 */
template<size_t From, size_t To, typename Parent>
struct tuple_view;

/**
 * \brief Tuple View (std::tuple) specialization
 */
template<size_t From, size_t To, typename ... Ts>
struct tuple_view<From, To, std::tuple<Ts...>>
    : tuple_slice_t<From, To, std::tuple<Ts...>> {

    constexpr static size_t from = From;
    constexpr static size_t to = To;

    /**
     * \brief The original tuple from which any tuple view or
     * a child of a tuple view was created
     */
    using original_type = std::tuple<Ts...>;
    using result_type = tuple_slice_t<From, To, original_type>;

    tuple_view(original_type& t)
        : tuple_slice_t<From, To, original_type>(tuple_slice<From, To>(t)),
            ref(t) {}

    std::tuple<Ts...>& ref;
};


template<size_t Index, typename TupleView>
struct tuple_element {};

template<size_t Index, size_t From, size_t To, typename Parent>
struct tuple_element<Index, tuple_view<From, To, Parent>> {
    static_assert(Index >= From, "Index is greater or equal to view from");
    static_assert(Index < To, "Index less than view to");
    using type = std::tuple_element_t<From + Index, typename tuple_view<From, To, Parent>::original_type>;
};

template<size_t Index, typename ... Ts>
struct tuple_element<Index, std::tuple<Ts...>> {
    using type = std::tuple_element_t<Index, std::tuple<Ts...>>;
};

/**
 * \brief Retrieve the tuple_view_element type
 */
template<size_t Index, typename TupleView>
using tuple_view_element_t = typename tuple_element<Index, TupleView>::type;

/**
 * \brief Slice a tuple into a tuple view
 * \see tuple_view
 */
template <std::size_t From, std::size_t To, typename ... Ts>
constexpr auto tuple_slice_view(std::tuple<Ts...>& t)
{
    static_assert(From <= To, "invalid slice");
    static_assert(sizeof...(Ts) >= To, "slice index out of bounds");

    return tuple_view<From, To, std::tuple<Ts...>>(t);
}

/**
 * \brief Slice a tuple into a tuple view (perfect-forwarding)
 * \see tuple_view
 */
template <std::size_t From, std::size_t To, typename ... Ts>
constexpr auto tuple_slice_view(std::tuple<Ts...>&& t)
{
    static_assert(From <= To, "invalid slice");
    static_assert(sizeof...(Ts) >= To, "slice index out of bounds");
    return tuple_view<From, To, std::tuple<Ts...>>{std::forward<std::tuple<Ts...>>(t)};
}

/**
 * \brief Slice a tuple into a tuple view
 * \see tuple_view
 */
template <std::size_t From, std::size_t To, typename ... Ts, size_t FromParent, size_t ToParent>
constexpr auto tuple_slice_view(tuple_view<FromParent, ToParent, Ts...>& t)
{
    static_assert(From <= To, "invalid slice");
    static_assert(sizeof...(Ts)-ToParent >= To, "slice index out of bounds");

    return tuple_view<FromParent + From, FromParent + To, Ts...>{t.ref};
}


/**
 * \brief Get the value at the specified Index of a tuple_view
 */
template<size_t Index, size_t From, size_t To, typename ... Ts>
inline auto& get(tuple_view<From, To, Ts...>& cont) {
    return std::get<Index>(cont);
}

/**
 * \brief Tuple View size helper class
 */
template<typename TupleView>
struct tuple_view_size {
    constexpr static size_t value = std::tuple_size_v<typename TupleView::result_type>;
};

/**
 * \brief Tuple View size helper class
 */
template<typename TupleView>
struct tuple_view_original_size {
    constexpr static size_t value = std::tuple_size_v<typename TupleView::original_type>;
};


/**
 * \brief Get the size of a tuple view (template variable)
 */
template<typename TupleView>
constexpr size_t tuple_view_size_v = tuple_view_size<TupleView>::value;

SP_UTIL_NAMESPACE_END

#endif /* SP_UTIL_TUPLES_HPP */