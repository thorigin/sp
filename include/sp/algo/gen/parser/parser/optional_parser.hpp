/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_OPTIONAL_PARSER_HPP
#define	SP_ALGO_PARSER_OPTIONAL_PARSER_HPP

#include "../core.hpp"
#include <optional>
#include "detail/attribute.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

/**
 * \brief Not Parser
 *
 * Example: -a
 */
template<typename Subject>
struct optional_parser
    : unary_parser<optional_parser<Subject>> {

    using subject_type = Subject;
    using subject_attribute_type = typename subject_type::attribute_type;
    using attribute_type = std::optional<subject_attribute_type>;

    optional_parser(subject_type&& subj) : subject(std::forward<subject_type>(subj)) {}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        Attribute cattr;
        if(subject.parse(reader, ctx, cattr)) {
            detail::assign(attr, std::move(cattr));
        }
        return true;
    }

    subject_type subject;
};

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_OPTIONAL_PARSER_HPP */

