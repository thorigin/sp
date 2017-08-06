/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_DIFF_PARSER_HPP
#define	SP_ALGO_PARSER_DIFF_PARSER_HPP

#include "../core.hpp"
#include "../detail/traits.hpp"
#include "detail/attribute.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

/**
 * \brief Difference Parser
 *
 * Example: a b
 */
template<typename Left, typename Right>
struct diff_parser
    : binary_parser<diff_parser<Left, Right>> {

    using left_type = Left;
    using right_type = Right;
    using attribute_type = void;

    diff_parser(left_type&& lhs, right_type&& rhs)
        : left_parser(std::forward<left_type>(lhs)),
            right_parser(std::forward<right_type>(rhs)){}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        auto p = reader.pos();
        if(right_parser.parse(reader, ctx, attr)) {
            reader.seek(p);
            return false;
        }
        return left_parser.parse(reader, ctx, attr);
    }

    left_type left_parser;
    right_type right_parser;
};


SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_CORE_HPP */

