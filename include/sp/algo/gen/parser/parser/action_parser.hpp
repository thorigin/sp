/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_ACTION_PARSER_HPP
#define	SP_ALGO_PARSER_ACTION_PARSER_HPP

#include "../core.hpp"
#include "../detail/traits.hpp"
#include "detail/attribute.hpp"
#include "detail/action.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

/**
 * \brief Action Parser
 *
 * Unary Parser that encapsulates the parser and executes an action
 * when the subject has a match
 */
template<typename Subject, typename Action>
struct action_parser
    : unary_parser<action_parser<Subject, Action>> {

    using subject_type = Subject;
    using action_type = Action;
    using attribute_type = typename subject_type::attribute_type;

    action_parser(const subject_type& subject, action_type&& act)
        : subject(subject), action(std::forward<action_type>(act)) {}

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

    subject_type subject;
    action_type action;

};

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_ACTION_PARSER_HPP */
