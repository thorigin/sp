/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_GE_PARSER_DETAIL_ACTION_HPP
#define	SP_ALGO_GEN_GE_PARSER_DETAIL_ACTION_HPP

#include <type_traits>

/**
 * \internal Move and actions semantics for parsers
 */
namespace sp { namespace algo { namespace gen { namespace parser { namespace detail {

template<
    typename Function,
    typename Context,
    std::enable_if_t<
        std::is_same<std::invoke_result_t<Function, Context&>, bool>::value,
        int
    > = 0
>
bool call(Function func, Context& ctx) {
    return func(ctx);
}

template<
    typename Function,
    typename Context,
    std::enable_if_t<
        std::is_same<std::invoke_result_t<Function, Context&>, void>::value,
        int
    > = 0
>
bool call(Function func, Context& ctx) {
    func(ctx);
    return true;
}

}}}}}


#endif	/* SP_ALGO_GEN_GE_PARSER_DETAIL_ACTION_HPP */

