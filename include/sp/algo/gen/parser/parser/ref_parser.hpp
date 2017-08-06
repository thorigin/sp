/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_REF_PARSER_HPP
#define	SP_ALGO_PARSER_REF_PARSER_HPP

#include "detail/action.hpp"
#include <type_traits>

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

/**
 * \brief Reference Parser
 *
 * References another parser
 */
template<typename Subject>
struct reference_parser
    : base_parser<reference_parser<Subject>> {

    using subject_type = Subject;
    using attribute_type = typename subject_type::attribute_type;

    reference_parser(Subject& subject) : ref(subject) {}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        return ref.get().parse_impl(reader, ctx, attr);
    }

    std::reference_wrapper<subject_type> ref;
};

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_REF_PARSER_HPP */

