/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_ALT_PARSER_HPP
#define	SP_ALGO_PARSER_ALT_PARSER_HPP

#include "../core.hpp"
#include "../detail/traits.hpp"
#include "detail/attribute.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

/**
 * \brief Alternative Parser
 *
 * Example: a | b
 */
template<typename Left, typename Right>
struct alt_parser
    : binary_parser<alt_parser<Left, Right>> {

    using left_type = Left;
    using right_type = Right;
    using left_attribute_type = typename left_type::attribute_type;
    using right_attribute_type = typename right_type::attribute_type;
    //using attribute_type = traits::tuple_to_variant_t<detail::collect_binary_parser_types_t<alt_parser<Left, Right>>>;
    using attribute_type = traits::tuple_to_variant_t<
        util::tuple_push_front_t<
            detail::collect_binary_parser_types_t<alt_parser<Left, Right>>,
            std::monostate
        >
    >;

    alt_parser(left_type&& lhs, right_type&& rhs)
        : left_parser(std::forward<left_type>(lhs)),
            right_parser(std::forward<right_type>(rhs)){}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        using strategy_type = detail::alternative_attribute_strategy<Left, Right, Attribute>;
        //Note: __attribute__((unused)) silences erroneous GCC warnings about being unused
        auto&& lattr __attribute__((unused)) = strategy_type::left(attr);
        auto&& rattr __attribute__((unused)) = strategy_type::right(attr);
        if(!(left_parser.parse(reader, ctx, lattr))) {
            right_attribute_type rattr;
            if(!right_parser.parse(reader, ctx, rattr)) {
                return false;
            } else {
                strategy_type::right_assign(attr, std::move(rattr));
            }
        }  else {
            strategy_type::left_assign(attr, std::move(lattr));
        }
        return true;
    }

    left_type left_parser;
    right_type right_parser;
};

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_ALT_PARSER_HPP */

