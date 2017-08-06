/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_LIST_PARSER_HPP
#define	SP_ALGO_PARSER_LIST_PARSER_HPP

#include "../core.hpp"
#include <vector>
#include <utility>

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

/**
 * \brief List Parser
 *
 * Example: a % b
 */
template<typename Left, typename Right>
struct list_parser
    : binary_parser<list_parser<Left, Right>> {

    using left_type = Left;
    using right_type = Right;
    using attribute_type = std::vector<typename left_type::attribute_type>;

    list_parser(left_type&& lhs, right_type&& rhs)
        : left_parser(std::forward<left_type>(lhs)),
            right_parser(std::forward<right_type>(rhs)){}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        empty_attribute e;
        if(!left_parser.parse(reader, ctx, e)) {
            return false;
        }
        auto p = reader.pos();
        while(right_parser.parse(reader, ctx, attr) && left_parser.parse(reader, ctx, attr)) {
            p = reader.pos();
        }
        reader.seek(p);
        return true;
    }

    left_type left_parser;
    right_type right_parser;
};

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_LIST_PARSER_HPP */

