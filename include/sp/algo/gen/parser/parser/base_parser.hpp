/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_PARSER_BASE_PARSER_HPP
#define	SP_ALGO_GEN_PARSER_BASE_PARSER_HPP

#include "../core.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

//Forward declaration
template <typename Subject, typename Action>
struct action_parser;

/**
 * \brief (Abstract) Parser
 */
template<typename Derived>
struct base_parser : parsable {

    using derived_type = Derived;

    /**
     * \brief Parse: evaluates parser and possibly consumes reader input (or a part) if successful
     *
     * \param reader
     * \param ctx The associated context
     * \param attr The associated attr
     */
    template<typename Reader, typename Context, typename Attribute>
    bool parse(Reader& reader, Context& ctx, Attribute& attr) const {
        return derived().parse_impl(reader, ctx, attr);
    }

    template<typename Reader, typename Attribute>
    bool parse(Reader& reader, Attribute& attr) const {
        context ctx;
        return derived().parse_impl(reader, ctx, attr);
    }

    template<typename Reader>
    bool parse(Reader& reader) const {
        empty_attribute empty;
        context ctx;
        return derived().parse_impl(reader, ctx, empty);
    }

    template <typename Action>
    action_parser<derived_type, Action>
    operator[](Action&& action) const
    {
        return action_parser<derived_type, Action>(this->derived(), std::forward<Action>(action));
    }

    template<typename Reader, typename Context, typename Attribute>
    std::string explain(Reader& reader) {
        return "N/A";
    }

    derived_type const& derived() const {
        return *static_cast<derived_type const*>(this);
    }
};

/**
 * \brief Value Parser
 */
template<typename Derived>
struct value_parser : base_parser<Derived> {

    using parser_group = value_parser_group;

};

/**
 * \brief Unary Parser
 */
template<typename Derived>
struct unary_parser : base_parser<Derived> {
    //using subject_type = subject parser

    using parser_group = unary_parser_group;
};

/**
 * \brief Unary Parser
 */
template<typename Derived>
struct binary_parser : base_parser<Derived> {
    //using left_type = left parser
    //using right_type = right parser

    using parser_group = binary_parser_group;
};

SP_ALGO_GEN_PARSER_NAMESPACE_END

#endif	/* SP_ALGO_GEN_PARSER_BASE_PARSER_HPP */

