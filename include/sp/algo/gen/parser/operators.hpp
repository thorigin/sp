/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_OPERATORS_HPP
#define	SP_ALGO_PARSER_HPP

#include "parsers.hpp"
#include "detail/as_parsable.hpp"
#include <boost/integer.hpp>
#include "parsers.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN


/****************************************************************************
 * Operator overloads:
 * Specifically chosen to mimick Boost Spirit operators to ease possible future
 * migration path or alternatives
 ****************************************************************************/

/**
 * \brief Action parser operator /
 */
template <typename Parser, typename Action>
action_parser<
    detail::enable_if_is_parsable_t<Parser>,
    Action
>
operator/(Parser&& parser, Action&& f) {
    return {
        detail::as_parsable(std::forward<Parser>(parser)),
        std::forward<Action>(f)
    };
}

/**
 * \brief Alternative Parser Operator |
 */
template<typename Left, typename Right>
alt_parser<
    detail::enable_if_is_parsable_t<Left>,
    detail::enable_if_is_parsable_t<Right>
>
operator|(Left&& lhs, Right&& rhs) {
    return {
        detail::as_parsable(std::forward<Left>(lhs)),
        detail::as_parsable(std::forward<Right>(rhs))
    };
}

/**
 * \brief And Predicate Parser Operator &
 */
template<typename Subject>
and_predicate_parser<
    detail::enable_if_is_parsable_t<Subject>
>
operator&(Subject&& subject) {
    return {detail::as_parsable(std::forward<Subject>(subject))};
}

/**
 * \brief Difference Parser Operator -
 */
template<typename Left, typename Right>
diff_parser<
    detail::enable_if_is_parsable_t<Left>,
    detail::enable_if_is_parsable_t<Right>
>
operator-(Left&& lhs, Right&& rhs) {
    return {
        detail::as_parsable(std::forward<Left>(lhs)),
        detail::as_parsable(std::forward<Right>(rhs))
    };
}

/**
 * \brief Kleene Parser Operator *
 */
template<typename Subject>
kleene_parser<
    detail::enable_if_is_parsable_t<Subject>
>
operator*(Subject&& subject) {
    return {detail::as_parsable(std::forward<Subject>(subject))};
}

/**
 * \brief List Parser Operator %
 */
template<typename Left, typename Right>
list_parser<
    detail::enable_if_is_parsable_t<Left>,
    detail::enable_if_is_parsable_t<Right>
>
operator%(Left&& lhs, Right&& rhs) {
    return {
        detail::as_parsable(std::forward<Left>(lhs)),
        detail::as_parsable(std::forward<Right>(rhs))
    };
}

/**
 * \brief Not Predicate Parser Operator !
 */
template<typename Subject>
not_predicate_parser<
    detail::enable_if_is_parsable_t<Subject>
>
operator!(Subject&& subject) {
    return {detail::as_parsable(std::forward<Subject>(subject))};
}

/**
 * \brief Optional Parser Operator -
 */
template<typename Subject>
optional_parser<
    detail::enable_if_is_parsable_t<Subject>
>
operator-(Subject&& subject) {
    return {detail::as_parsable(std::forward<Subject>(subject))};
}

/**
 * \brief Plus Parser Operator +
 */
template<typename Subject>
plus_parser<
    detail::enable_if_is_parsable_t<Subject>
>
operator+(Subject&& subject) {
    return {detail::as_parsable(std::forward<Subject>(subject))};
}

/**
 * \brief Sequence Parser Operator >>
 */
template<typename Left, typename Right>
seq_parser<
    detail::enable_if_is_parsable_t<Left>,
    detail::enable_if_is_parsable_t<Right>
>
operator>>(Left&& lhs, Right&& rhs) {
    return {
        detail::as_parsable(std::forward<Left>(lhs)),
        detail::as_parsable(std::forward<Right>(rhs))
    };
}

template<typename T>
std::enable_if_t<std::is_integral_v<T>>
lit(T literal) {
    return bit_string_value_parser<T>(literal);
}

namespace detail {

    /**
     * \brief Attempt to convert right hand side into left hand as an assignable value
     * IFF Right_type is NOT assignable to Left_type
     * and Right type is iterable of bools
     * and Left type is
     */
    template<typename Left>
    constexpr Left
    as_integral(std::initializer_list<bool>&& right) {
        Left res = 0;
        Left i = 0;
        for(auto it = std::rbegin(right), end = std::rend(right); it != end; ++it, ++i) {
            res |= static_cast<Left>(*it) << i;
        }
        return res;
    }
}

template<bool ... Bits, typename Integral = typename boost::uint_t<sizeof...(Bits)>::fast>
auto lit() {
    BOOST_ASSERT_MSG(sizeof...(Bits) > 0, "Bits template parameters must be specified with a minimum of 1");
    constexpr size_t bits_used = sizeof...(Bits);
    using arr_type = std::array<bool, bits_used>;
    return bit_string_value_parser<Integral, bits_used>(
        detail::as_integral<Integral>({Bits...}), arr_type{Bits...}
    );
}

auto dummy(int val = 0) {
    return dummy_parser{val};
}

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_HPP */

