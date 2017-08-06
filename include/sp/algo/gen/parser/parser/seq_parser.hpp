/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_SEQ_PARSER_HPP
#define	SP_ALGO_PARSER_SEQ_PARSER_HPP

#include <vector>
#include <utility>
#include "../core.hpp"
#include "detail/attribute.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

/**
 * \brief Sequence Parser
 *
 * Example: a b
 */
template<typename Left, typename Right>
struct seq_parser : binary_parser<seq_parser<Left, Right>> {

    using left_type = Left;
    using right_type = Right;
    using attribute_type = detail::collect_binary_parser_types_t<seq_parser>;

    seq_parser(left_type&& lhs, right_type&& rhs)
        : left_parser(std::forward<left_type>(lhs)),
            right_parser(std::forward<right_type>(rhs)){}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        auto p = reader.pos();
        using strategy_type = detail::sequence_attribute_strategy<left_type, right_type, Attribute>;
        auto&& lattr = strategy_type::left(attr);
        auto&& rattr = strategy_type::right(attr);
        if(!(left_parser.parse(reader, ctx, lattr) && right_parser.parse(reader, ctx, rattr))) {
            reader.seek(p);
            return false;
        }
        strategy_type::left_assign(attr, std::move(lattr));
        strategy_type::right_assign(attr, std::move(rattr));
        return true;
    }

    left_type left_parser;
    right_type right_parser;
};

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_SEQ_PARSER_HPP */

