/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_CORE_HPP
#define	SP_ALGO_PARSER_CORE_HPP

#include "sp/config.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

/**
 * \brief Generic parsing context
 */
struct context {};

/**
 * \brief Empty Attribute tag
 */
struct empty_attribute {};

/**
 * \brief Container Attribute tag
 */
struct container_attribute {};

/**
 * \brief Value Attribute tag
 */
struct value_attribute {};

/**
 * \brief Parsable
 *
 * Base tag for all parsers
 */
struct parsable {

    //template<typename Reader, typename Context, typename Attribute>
    //bool parse(Reader& reader, Context& ctx, Attribute& attr) const;

};

/**
 * \brief Group tag
 */
struct parser_group {};
struct binary_parser_group {};
struct unary_parser_group {};
struct nary_parser_group {};
struct value_parser_group {};


SP_ALGO_GEN_PARSER_NAMESPACE_END

#endif	/* SP_ALGO_PARSER_CORE_HPP */

