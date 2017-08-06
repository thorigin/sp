/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_DUMMY_PARSER_HPP
#define	SP_ALGO_PARSER_DUMMY_PARSER_HPP

#include "../core.hpp"
#include "../detail/traits.hpp"
#include "detail/attribute.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

/**
 * \brief Dummy value wrapper for testing
 */
struct dummy_value {
    int value = 0;
};

/**
 * \brief shorthand for brevity for a dummy value
 */
using dv = dummy_value;

/**
 * \brief Dummy parser, for testing purposes - Always succeeds
 */
struct dummy_parser
    : base_parser<dummy_parser> {

    using attribute_type = dummy_value;

    dummy_parser() {}

    dummy_parser(int val) : value{val} {}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        detail::assign(attr, value);
        return true;
    }

    attribute_type value;
};

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_DUMMY_PARSER_HPP */

