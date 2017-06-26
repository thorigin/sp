/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_GE_PARSER_PARSERS_HPP
#define	SP_ALGO_GEN_GE_PARSER_PARSERS_HPP

#include "detail/traits.hpp"
#include "detail/attribute.hpp"
#include <vector>
#include <tuple>
#include <bitset>
#include <variant>
#include <optional>

/**
 * \brief Provides for a recursive descendent parser to be used
 * for genetic expressions
 */
namespace sp { namespace algo { namespace gen { namespace parser {

/**
 * \brief Alternative Parser
 *
 * Example: a | b
 */
template<typename Left, typename Right>
struct alt_parser : binary_parser<alt_parser<Left, Right>> {

    using left_type = Left;
    using right_type = Right;
    using left_attribute_type = typename left_type::attribute_type;
    using right_attribute_type = typename right_type::attribute_type;
    using attribute_type = traits::tuple_to_variant_t<detail::collect_binary_parser_types_t<alt_parser<Left, Right>>>;

    alt_parser(left_type&& lhs, right_type&& rhs)
        :   left_parser(std::forward<left_type>(lhs)),
            right_parser(std::forward<right_type>(rhs)){}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        using strategy_type = detail::alternative_attribute_strategy<Left, Right, Attribute>;
        //Note: __attribute__((unused)) silences erroneous GCC warnings about being unused
        auto&& lattr __attribute__((unused)) = strategy_type::left(attr);
        auto&& rattr __attribute__((unused)) = strategy_type::right(attr);
        if(!(left_parser.parse(reader, ctx, lattr))) {
            right_attribute_type rattr;
            if(!right_parser.parse(reader, ctx, rattr)) {
                return false;
            } else {
                strategy_type::right_assign(attr, std::move(rattr));
            }
        }  else {
            strategy_type::left_assign(attr, std::move(lattr));
        }
        return true;
    }

    left_type left_parser;
    right_type right_parser;
};

/**
 * \brief And Predicate Parser
 *
 * Example: &a
 */
template<typename Subject>
struct and_predicate_parser : unary_parser<and_predicate_parser<Subject>> {

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

/**
 * \brief Difference Parser
 *
 * Example: a b
 */
template<typename Left, typename Right>
struct diff_parser : binary_parser<diff_parser<Left, Right>> {

    using left_type = Left;
    using right_type = Right;
    using attribute_type = void;

    diff_parser(left_type&& lhs, right_type&& rhs)
        :   left_parser(std::forward<left_type>(lhs)),
            right_parser(std::forward<right_type>(rhs)){}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        auto p = reader.pos();
        if(right_parser.parse(reader, ctx, attr)) {
            reader.seek(p);
            return false;
        }
        return left_parser.parse(reader, ctx, attr);
    }

    left_type left_parser;
    right_type right_parser;
};

/**
 * \brief Kleene Parser
 *
 * Example: *a
 */
template<typename Subject>
struct kleene_parser : unary_parser<kleene_parser<Subject>> {

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

/**
 * \brief List Parser
 *
 * Example: a % b
 */
template<typename Left, typename Right>
struct list_parser : binary_parser<list_parser<Left, Right>> {

    using left_type = Left;
    using right_type = Right;
    using attribute_type = std::vector<typename left_type::attribute_type>;

    list_parser(left_type&& lhs, right_type&& rhs)
        :   left_parser(std::forward<left_type>(lhs)),
            right_parser(std::forward<right_type>(rhs)){}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        if(!left_parser.parse(reader, ctx, attr)) {
            return false;
        }
        auto p = reader.pos();
        while(right_parser.parse(reader, ctx, attr) && left_parser.parse(reader, ctx, attr)) {
            p = reader.pos();
        }
        reader.seek(p);
        return true;
    }

    left_type left_parser;
    right_type right_parser;
};

/**
 * \brief Not Parser
 *
 * Example: !a
 */
template<typename Subject>
struct not_predicate_parser : unary_parser<not_predicate_parser<Subject>> {

    using subject_type = Subject;
    using attribute_type = void;

    not_predicate_parser(subject_type&& subj) : subject(std::forward<subject_type>(subj)) {}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        auto p = reader.pos();
        empty_attribute empty;
        auto result = !subject.parse(reader, ctx, empty);
        reader.seek(p);
        return result;
    }

    subject_type subject;
};


/**
 * \brief Not Parser
 *
 * Example: -a
 */
template<typename Subject>
struct optional_parser : unary_parser<optional_parser<Subject>> {

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

/**
 * \brief Plus Parser
 *
 * Example: +a
 */
template<typename Subject>
struct plus_parser : unary_parser<plus_parser<Subject>> {

    using subject_type = Subject;
    using attribute_type = std::vector<typename subject_type::attribute_type>;

    plus_parser(subject_type&& subj) : subject(std::forward<subject_type>(subj)) {}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        if(!subject.parse(reader, ctx, attr)) {
            return false;
        }
        while(subject.parse(reader, ctx, attr));
        return true;
    }

    subject_type subject;
};

/**
 * \brief Sequence Parser
 *
 * Example: a b
 */
template<typename Left, typename Right>
struct seq_parser : binary_parser<seq_parser<Left, Right>> {

    using left_type = Left;
    using right_type = Right;
    using attribute_type = detail::collect_binary_parser_types_t<seq_parser>;

    seq_parser(left_type&& lhs, right_type&& rhs)
        :   left_parser(std::forward<left_type>(lhs)),
            right_parser(std::forward<right_type>(rhs)){}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        auto p = reader.pos();
        using strategy_type = detail::sequence_attribute_strategy<left_type, right_type, Attribute>;
        auto&& lattr = strategy_type::left(attr);
        auto&& rattr = strategy_type::right(attr);
        if(!(left_parser.parse(reader, ctx, lattr) && right_parser.parse(reader, ctx, rattr))) {
            reader.seek(p);
            return false;
        }
        strategy_type::left_assign(attr, std::move(lattr));
        strategy_type::right_assign(attr, std::move(rattr));
        return true;
    }

    left_type left_parser;
    right_type right_parser;
};

namespace detail {

    //extension points for adapting a value to a boolean vector

    template<typename T>
    constexpr size_t integral_bit_count_v = std::is_same_v<T, bool> ? 1 : sizeof(T)*CHAR_BIT;

    template<typename T, size_t Length = integral_bit_count_v<T>, typename = void>
    struct as_iterable_array {

        constexpr static size_t length = Length;
        using type = std::array<bool, length>;

        static type apply(const T& value) {
            std::bitset<length> bs(value);
            type result{};
            for(int i = length-1; i >= 0; --i) {
                size_t ri = length - i - 1;
                result[i] = bs[ri];
            }
            return result;
        }
    };

    template<typename T>
    constexpr
    std::enable_if_t<std::is_integral_v<T>, typename as_iterable_array<T>::type>
    as_iterable(const T& value) {
        return as_iterable_array<T>::apply(value);
    }

    template<typename T, size_t N>
    constexpr auto
    as_iterable(std::array<T, N>&& value) {
        return std::forward<std::array<T, N>>(value);
    }
}


/**
 * \brief Abstract Binary Value Parser
 *
 * Matches input to a specific binary value
 */
template<typename AttributeValue, typename Derived, size_t Bits>
struct abstract_binary_value_parser : value_parser<Derived> {

    using attribute_type = AttributeValue;

    using iterable_type = std::array<bool, Bits>;

    abstract_binary_value_parser(AttributeValue&& val)
        :   value(std::forward<AttributeValue>(val)), iterable_value(detail::as_iterable(value)) {}

    abstract_binary_value_parser(AttributeValue&& val, iterable_type&& iterable)
        :   value(std::forward<AttributeValue>(val)), iterable_value(std::forward<iterable_type>(iterable)) {}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        auto p = reader.pos();
        bool success = true;
        for(auto it = std::begin(iterable_value), end = std::end(iterable_value); it != end; ++it) {
            bool l = *it;
            if(!(reader.template has_next<1>() && reader.template next<1, bool>() == l)) {
                success = false;
                break;
            }
        }
        if(!success) {
            reader.seek(p);
            return false;
        } else {
            detail::assign(attr, attribute_type(value));
            return true;
        }
    }

    attribute_type value;
    iterable_type iterable_value;
};


/**
 * \brief Abstract Binary Size Parser
 *
 * Matches input to a specific binary value size
 */
template<typename AttributeValue, typename Derived, size_t Bits>
struct abstract_binary_size_parser : value_parser<Derived> {

    using attribute_type = AttributeValue;

    constexpr static size_t bits_used = Bits;

    abstract_binary_size_parser() {}

    template<typename Reader, typename Context, typename Attribute>
    bool parse_impl(Reader& reader, Context& ctx, Attribute& attr) const {
        if(!reader.template has_next<bits_used>()) {
            return false;
        }
        attribute_type result = reader.template next<bits_used, attribute_type>();
        detail::assign(attr, result);
        return true;
    }

};



/**
 * \brief Bit String Value Parser
 *
 * \remark Matches a defined vector of bits to input
 */
template<typename T, size_t Bits = detail::integral_bit_count_v<T>>
struct bit_string_value_parser
    :   abstract_binary_value_parser<
            T,
            bit_string_value_parser<T, Bits>,
            Bits
        > {

    bit_string_value_parser(T&& value)
        :   abstract_binary_value_parser<
                T,
                bit_string_value_parser<T, Bits>,
                Bits
            >(std::forward<T>(value)) {}

    bit_string_value_parser(T&& value, std::array<bool, Bits>&& iterable)
        :   abstract_binary_value_parser<
                T,
                bit_string_value_parser<T, Bits>,
                Bits
            >(std::forward<T>(value), std::forward<std::array<bool, Bits>>(iterable)) {}
};



/**
 * \brief Integral Value Parser
 *
 * \remark matches input a size
 */
template<typename T, size_t Bits = detail::integral_bit_count_v<T>>
struct integral_value_parser
    :   abstract_binary_size_parser<
            T,
            integral_value_parser<T, Bits>,
            Bits
        > {

    integral_value_parser()
        :   abstract_binary_size_parser<
                T,
                integral_value_parser<T, Bits>,
                Bits
            >() {}
};


/**
 * \brief Helper macros for data type specific parser definition
 * and helper function
 *
 * SP_GEN_PARSER_REG_BS_VAL_PARSER creates:
 * - <name>_value_parser
 * - <name>_val(<type>&& val) helper function
 * - detail::as_parsable(<type>&& val) specialization for type
 *
 * SP_GEN_PARSER_REG_INT_PARSER creates:
 * - <name>-_parser
 * - <name>_any helper function which returns an instance of <name>_parser
 * - detail::as_parsable(<type>&&val) specialization for type
 */
#define SP_GEN_PARSER_REG_BS_VAL_PARSER_FUNC(name) name ## _val
#define SP_GEN_PARSER_REG_BS_VAL_PARSER_TYPE(name) name ## _value_parser
#define SP_GEN_PARSER_REG_BS_VAL_PARSER(type, name)                             \
    using SP_GEN_PARSER_REG_BS_VAL_PARSER_TYPE(name)                            \
            = bit_string_value_parser<type>;                                    \
    inline  SP_GEN_PARSER_REG_BS_VAL_PARSER_TYPE(name)                          \
            SP_GEN_PARSER_REG_BS_VAL_PARSER_FUNC(name)(type&& val) {            \
        return { std::forward<type>(val) };                                     \
    }                                                                           \
    namespace detail {                                                          \
        SP_GEN_PARSER_REG_BS_VAL_PARSER_TYPE(name)                              \
        as_parsable(type&& val) {                                               \
            return SP_GEN_PARSER_REG_BS_VAL_PARSER_TYPE(name) {                 \
                std::forward<type>(val)                                         \
            };                                                                  \
        }                                                                       \
    }

#define SP_GEN_PARSER_REG_INT_PARSER_FUNC(name) name ## _any
#define SP_GEN_PARSER_REG_INT_PARSER_TYPE(name) name ## _parser
#define SP_GEN_PARSER_REG_INT_PARSER(type, name)                                \
    template<size_t Bits = detail::integral_bit_count_v<type>>                    \
    using SP_GEN_PARSER_REG_INT_PARSER_TYPE(name)                               \
            = integral_value_parser<type, Bits>;                           \
    inline  SP_GEN_PARSER_REG_INT_PARSER_TYPE(name)<>                            \
            SP_GEN_PARSER_REG_INT_PARSER_FUNC(name)() {                         \
        return {};                                                              \
    }                                                                           \
    template<size_t Bits = detail::integral_bit_count_v<type>>                     \
    inline  SP_GEN_PARSER_REG_INT_PARSER_TYPE(name)<Bits>                  \
            SP_GEN_PARSER_REG_INT_PARSER_FUNC(name)() {                         \
        return {};                                                              \
    }


SP_GEN_PARSER_REG_BS_VAL_PARSER(int8_t, int8)
SP_GEN_PARSER_REG_BS_VAL_PARSER(int16_t, int16)
SP_GEN_PARSER_REG_BS_VAL_PARSER(int32_t, int32)
SP_GEN_PARSER_REG_BS_VAL_PARSER(int64_t, int64)
SP_GEN_PARSER_REG_BS_VAL_PARSER(uint8_t, uint8)
SP_GEN_PARSER_REG_BS_VAL_PARSER(uint16_t, uint16)
SP_GEN_PARSER_REG_BS_VAL_PARSER(uint32_t, uint32)
SP_GEN_PARSER_REG_BS_VAL_PARSER(uint64_t, uint64)
SP_GEN_PARSER_REG_BS_VAL_PARSER(bool, bool)

SP_GEN_PARSER_REG_INT_PARSER(int8_t, int8)
SP_GEN_PARSER_REG_INT_PARSER(int16_t, int16)
SP_GEN_PARSER_REG_INT_PARSER(int32_t, int32)
SP_GEN_PARSER_REG_INT_PARSER(int64_t, int64)
SP_GEN_PARSER_REG_INT_PARSER(uint8_t, uint8)
SP_GEN_PARSER_REG_INT_PARSER(uint16_t, uint16)
SP_GEN_PARSER_REG_INT_PARSER(uint32_t, uint32)
SP_GEN_PARSER_REG_INT_PARSER(uint64_t, uint64)
SP_GEN_PARSER_REG_INT_PARSER(bool, bool)

namespace detail {

    /**
     * \brief Rule Capture (functor)
     */
    template<typename Parser>
    struct rule_capture {

        using attribute_type = typename Parser::attribute_type;
        using parser_type = Parser;

        rule_capture(parser_type&& parser)
            :   parser(std::forward<parser_type>(parser)) {}

        template<typename Reader, typename Context, typename Attribute>
        bool operator()(Reader& reader, Context& ctx, Attribute& attr) const {
            if(parser.parse(reader, ctx, attr)) {
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



/**
 * \brief Dummy parser, for testing purposes - Always succeeds
 */
struct dummy_parser : base_parser<dummy_parser> {

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

}}}}


#endif	/* SP_ALGO_GEN_GE_PARSER_PARSERS_HPP */

