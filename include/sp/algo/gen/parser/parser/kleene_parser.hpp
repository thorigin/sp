/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_KLEENE_PARSER_HPP
#define	SP_ALGO_PARSER_KLEENE_PARSER_HPP

#include <vector>
#include <utility>
#include "../core.hpp"


SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

/**
 * \brief Kleene Parser
 *
 * Example: *a
 */
template<typename Subject>
struct kleene_parser
    : unary_parser<kleene_parser<Subject>> {

    using subject_type = Subject;
    using attribute_type = std::vector<typename subject_type::attribute_type>;

    kleene_parser(subject_type&& subj) : subject(std::forward<subject_type>(subj)) {}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        while(subject.parse(reader, ctx, attr));
        return true;
    }

    subject_type subject;
};

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_KLEENE_PARSER_HPP */

