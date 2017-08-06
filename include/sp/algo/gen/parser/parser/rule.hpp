/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_RULE_HPP
#define	SP_ALGO_PARSER_RULE_HPP

#include <string>
#include <functional>
#include <utility>
#include "../core.hpp"
#include "detail/attribute.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

namespace detail {

    /**
     * \brief Rule Capture (functor)
     */
    template<typename Parser>
    struct rule_capture {

        using attribute_type = typename Parser::attribute_type;
        using parser_type = Parser;

        rule_capture(parser_type&& parser)
            : parser(std::forward<parser_type>(parser)) {}

        template<typename Reader, typename Context, typename Attribute>
        bool operator()(Reader& reader, Context& ctx, Attribute& attr) const {
            static_assert(
                std::is_constructible_v<Attribute, attribute_type> ||
                std::is_assignable_v<attribute_type, Attribute>,
                "Rule attribute must be constructible from parser definition attribute"
            );
            attribute_type rattr;
            if(parser.parse(reader, ctx, rattr)) {
                detail::assign(attr, std::move(rattr));
                return true;
            }
            return false;
        }

        parser_type parser;
    };

}

/**
 * \brief Rule
 *
 * \tparam Attribute (optional) attribute type
 */
template<typename Reader, typename Attribute = empty_attribute>
struct rule : base_parser<rule<Reader, Attribute>> {

    using reader_type = Reader;
    using attribute_type = Attribute;
    using context_type = context;
    using definition_type = std::function<bool(reader_type&, context_type&, attribute_type&)>;

    static_assert(std::is_default_constructible_v<Attribute>, "Attribute must be default constructible");

    rule() : name("no-name-rule"), def() {}
    rule(std::string rule_name) : name(rule_name), def() {}

    template<typename Reader_, typename Context>
    bool parse_impl(Reader_& reader, Context& ctx, Attribute& attr) const {
        if(def) {
            if(def(reader, ctx, attr)) {
                return true;
            }
        }
        return false;
    }

    template<typename Parser>
    void operator%=(Parser&& right) {
        def = detail::rule_capture<Parser>(std::forward<Parser>(right));
    }

    std::string name;
    definition_type def;
};

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_RULE_HPP */

