/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_GE_PARSER_CORE_HPP
#define	SP_ALGO_GEN_GE_PARSER_CORE_HPP

#include "detail/action.hpp"
#include <type_traits>

namespace sp { namespace algo { namespace gen { namespace parser {

/**
 * \brief Generic parsing context
 */
struct context {
    
};

template<size_t Value>
struct id {
    constexpr static size_t value = Value;
};

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
 * \todo Consider removing, unused?
 */
struct parser_group {};
struct binary_parser_group {};
struct unary_parser_group {};
struct nary_parser_group {};
struct value_parser_group {};


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

/**
 * \brief Action Parser
 *
 * Unary Parser that encapsulates the parser and executes an action
 * when the subject has a match
 */
template<typename Subject, typename Action>
struct action_parser : unary_parser<action_parser<Subject, Action>> {

    using subject_type = Subject;
    using action_type = Action;
    using attribute_type = typename subject_type::attribute_type;

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        if(subject.parse(reader, ctx, attr)) {
            /**
             * If there's an error calling action,then the signature does not
             * match any valid syntax for semantic actions
             */
            bool match = detail::call(action, ctx);
            if(match) {
                return true;
            }
        }
        return false;
    }

    action_parser(const subject_type& subject, action_type&& act)
        :   subject(subject), action(std::forward<action_type>(act)) {}

    subject_type subject;
    action_type action;

};

/**
 * \brief Reference Parser
 *
 * References another parser
 */
template<typename Subject>
struct reference_parser : base_parser<reference_parser<Subject>> {

    using subject_type = Subject;
    using attribute_type = typename subject_type::attribute_type;

    reference_parser(Subject& subject) : ref(subject) {}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        return ref.get().parse_impl(reader, ctx, attr);
    }

    std::reference_wrapper<subject_type> ref;
};

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

}}}}


#endif	/* SP_ALGO_GEN_GE_PARSER_CORE_HPP */

