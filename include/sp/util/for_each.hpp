/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_UTIL_FOR_EACH_HPP
#define SP_UTIL_FOR_EACH_HPP

#include <functional>
#include <tuple>
#include "sp/util/hints.hpp"

#include "types.hpp"

SP_UTIL_NAMESPACE_BEGIN

namespace detail {

    template<typename ... T, typename UnaryFunction, size_t ... Indexes>
    sp_hot void for_each_tuple_helper(std::tuple<T...>& tuple, UnaryFunction func, std::index_sequence<Indexes...>) {
        ((func(std::get<Indexes>(tuple))), ...);
    }

    template<typename ... T, typename UnaryFunction>
    sp_hot void for_each_tuple(std::tuple<T...>& tuple, UnaryFunction func) {
        for_each_tuple_helper(tuple, func, std::index_sequence_for<T...>{});
    }
}

/**
 * \brief Apply UnaryFunction  (func) to every element in the range of [start, end).
 */
template<typename Iterator, typename UnaryFunction>
sp_hot void for_each(Iterator start, Iterator end, UnaryFunction func) {
    std::for_each(start, end, func);
}

/**
 * \brief Apply UnaryFunction (func) to every element in the range specified
 */
template<typename Range, typename UnaryFunction>
sp_hot
enable_if_range_t<Range>
for_each(Range& range, UnaryFunction func) {
    std::for_each(std::begin(range), std::end(range), func);
}

/**
 * \brief Apply UnaryFunction (func) to every element in the tuple
 */
template<typename Tuple, typename UnaryFunction>
sp_hot
enable_if_tuple_t<Tuple>
for_each(Tuple&& tuple, UnaryFunction func) {
    detail::for_each_tuple(tuple, func);
}

SP_UTIL_NAMESPACE_END

#endif /* SP_UTIL_FOR_EACH_HPP */

