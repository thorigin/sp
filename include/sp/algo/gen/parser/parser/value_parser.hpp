/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_VALUE_PARSER_HPP
#define	SP_ALGO_PARSER_VALUE_PARSER_HPP

#include <vector>
#include <utility>
#include "../core.hpp"
#include "detail/attribute.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

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

    template<typename T, size_t Bits = integral_bit_count_v<T>>
    constexpr
    std::enable_if_t<std::is_integral_v<T>, typename as_iterable_array<T, Bits>::type>
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
        : value(std::forward<AttributeValue>(val)), iterable_value(detail::as_iterable<attribute_type, Bits>(value)) {}

    abstract_binary_value_parser(AttributeValue&& val, iterable_type&& iterable)
        : value(std::forward<AttributeValue>(val)), iterable_value(std::forward<iterable_type>(iterable)) {}

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
    : abstract_binary_value_parser<
            T,
            bit_string_value_parser<T, Bits>,
            Bits
        > {

    bit_string_value_parser(T&& value)
        : abstract_binary_value_parser<
                T,
                bit_string_value_parser<T, Bits>,
                Bits
            >(std::forward<T>(value)) {}

    bit_string_value_parser(T&& value, std::array<bool, Bits>&& iterable)
        : abstract_binary_value_parser<
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
    : abstract_binary_size_parser<
            T,
            integral_value_parser<T, Bits>,
            Bits
        > {

    integral_value_parser()
        : abstract_binary_size_parser<
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
    template<size_t Bits = detail::integral_bit_count_v<type>>                  \
    using SP_GEN_PARSER_REG_BS_VAL_PARSER_TYPE(name)                            \
            = bit_string_value_parser<type, Bits>;                              \
    template<size_t Bits = detail::integral_bit_count_v<type>>                  \
    inline  SP_GEN_PARSER_REG_BS_VAL_PARSER_TYPE(name)<Bits>                    \
            SP_GEN_PARSER_REG_BS_VAL_PARSER_FUNC(name)(type&& val) {            \
        return { std::forward<type>(val) };                                     \
    }                                                                           \
    namespace detail {                                                          \
        SP_GEN_PARSER_REG_BS_VAL_PARSER_TYPE(name)<>                            \
        as_parsable(type&& val) {                                               \
            return SP_GEN_PARSER_REG_BS_VAL_PARSER_TYPE(name)<> {               \
                std::forward<type>(val)                                         \
            };                                                                  \
        }                                                                       \
    }

#define SP_GEN_PARSER_REG_INT_PARSER_FUNC(name) name ## _any
#define SP_GEN_PARSER_REG_INT_PARSER_TYPE(name) name ## _parser
#define SP_GEN_PARSER_REG_INT_PARSER(type, name)                                \
    template<size_t Bits = detail::integral_bit_count_v<type>>                  \
    using SP_GEN_PARSER_REG_INT_PARSER_TYPE(name)                               \
            = integral_value_parser<type, Bits>;                                \
    inline  SP_GEN_PARSER_REG_INT_PARSER_TYPE(name)<>                           \
            SP_GEN_PARSER_REG_INT_PARSER_FUNC(name)() {                         \
        return {};                                                              \
    }                                                                           \
    template<size_t Bits = detail::integral_bit_count_v<type>>                  \
    inline  SP_GEN_PARSER_REG_INT_PARSER_TYPE(name)<Bits>                       \
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

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_VALUE_PARSER_HPP */

