/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_GE_PARSER_DETAIL_AS_PARSABLE_HPP
#define	SP_ALGO_GEN_GE_PARSER_DETAIL_AS_PARSABLE_HPP

#include "traits.hpp"

/**
 * \brief Provides for a recursive descendent parser to be used
 * for genetic expressions
 */
namespace sp { namespace algo { namespace gen { namespace parser { namespace detail {

/**
* \brief Is Parsable Type
* Determines whether or not a type can be converted to parsable
*/
template<typename T>
constexpr bool is_parsable_v = std::is_base_of<parsable, std::decay_t<T>>::value || std::is_integral_v<T>;

/**
 * \brief Activates operator overload and deduces the type of which T should
 * become
 *
 * \remark A reference will be wrapped into reference_parser, if parsable, otherwise
 * will result in simply type T
 */
template<typename T>
using enable_if_is_parsable_t = std::enable_if_t<
is_parsable_v<T>,
    std::conditional_t<
        std::is_reference_v<T>,
        reference_parser<std::decay_t<T>>,
        T
    >
>;

/**
 * \brief As parsable delegate functionality
 * * Default it forwards value
 */
template<typename T>
auto as_parsable(T&& val) {
    return std::forward<T>(val);
}

/**
 * \brief H
 */
template<typename T>
auto as_parsable(T& val) {
    return reference_parser<T>(val);
}

/**
 * \brief H
 */
template<typename T>
std::enable_if_t<std::is_integral_v<T>, T>
as_parsable(T&& val) {
    return bool_val(std::forward<bool>(val));
}

}}}}}

#endif	/* SP_ALGO_GEN_GE_PARSER_DETAIL_AS_PARSABLE_HPP */

