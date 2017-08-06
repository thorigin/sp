/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_AND_PREDICATE_PARSER_HPP
#define	SP_ALGO_PARSER_AND_PREDICATE_PARSER_HPP

#include "../core.hpp"
#include "../detail/traits.hpp"
#include "detail/attribute.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

/**
 * \brief And Predicate Parser
 *
 * Example: &a
 */
template<typename Subject>
struct and_predicate_parser
    : unary_parser<and_predicate_parser<Subject>> {

    using subject_type = Subject;
    using attribute_type = void;

    and_predicate_parser(subject_type&& subj) : subject(std::forward<subject_type>(subj)) {}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        auto p = reader.pos();
        auto result = !subject.parse(reader, ctx, attr);
        reader.seek(p);
        return result;
    }

    subject_type subject;
};

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_AND_PREDICATE_PARSER_HPP */

