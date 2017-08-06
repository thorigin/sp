/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_DETAIL_ATTRIBUTE_HPP
#define	SP_ALGO_PARSER_DETAIL_ATTRIBUTE_HPP

#include "../../core.hpp"
#include "sp/util/tuples.hpp"
#include "../../detail/traits.hpp"

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

//Forward declared for specialization in sequence_size, and is_sequence
template<typename Left_type, typename Right_type>
struct seq_parser;
//forward declared for specialization in is_alternative
template<typename Left_type, typename Right_type>
struct alt_parser;

namespace detail {

/**
 * \brief Assign empty attribute to destination (no-op)
 */
template<typename Dest>
inline void assign(Dest& dest, empty_attribute) {}

/**
 * \brief Assign empty attribute to empty attribute (no-op)
 */
inline void assign(empty_attribute, empty_attribute) {}

/**
 * \brief Assign empty attribute to empty attribute (no-op)
 */
template<typename Source>
inline void assign(empty_attribute, Source&& src) {}

/**
 * \brief Assign source to destination (assignment)
 */
template<typename Dest, typename Source>
inline
std::enable_if_t<
    !traits::is_vector_v<Dest>
>
assign(Dest& dest, Source&& src) {
    dest = std::forward<Source>(src);
}

/**
 * \brief Assign source to destination (assignment)
 */
template<typename Dest, typename Source>
inline
std::enable_if_t<
    traits::is_vector_v<Dest>
>
assign(Dest& dest, Source&& src) {
    dest.emplace_back(std::forward<Source>(src));
}

/**
 * \brief Assign source to destination of the same type (assignment)
 */
template<typename T>
inline void assign(T& dest, T&& src) {
    dest = std::forward<T>(src);
}

template<typename Dest, typename Iterator>
inline void assign(Dest& dest, Iterator first, Iterator last) {
    //verify that we're actually attempting to insert into a container (vector, list, etc)
    static_assert(traits::is_container_v<Dest>, "Destination is a container");
    if(dest.empty()) {
        dest = {first, last};
    } else {
        dest.insert(std::end(dest), first, last);
    }
}

template<typename Dest, typename Source>
inline void assign(Dest& dest, Source&& src, container_attribute) {
    return assign(dest, std::begin(src), std::end(src));
}

template<typename>
struct is_alternative : std::false_type {};

template<typename Left, typename Right>
struct is_alternative<alt_parser<Left, Right>> : std::true_type {};

template<typename T>
constexpr bool is_alternative_v = is_alternative<T>::value;

/**
 * \brief Alternative Attribute Strategy
 */
template<typename Left, typename Right, typename Attribute, typename = void>
struct alternative_attribute_strategy;

template<typename Left, typename Right, typename Attribute>
struct alternative_attribute_strategy<
    Left, Right, Attribute,
    std::enable_if_t<
        std::is_same_v<Attribute, empty_attribute>
    >
> {
    static empty_attribute left(empty_attribute) { return {}; }
    static empty_attribute right(empty_attribute) { return {}; }
    template<typename Source>
    static void left_assign(empty_attribute, Source&& src) {}
    template<typename Source>
    static void right_assign(empty_attribute, Source&& src) {}
};

template<typename Left, typename Right, typename Attribute>
struct alternative_attribute_strategy<
    Left, Right, Attribute,
    std::enable_if_t<
        !std::is_same_v<Attribute, empty_attribute> &&
        is_alternative_v<Left> && is_alternative_v<Right>
    >
> {
    using attribute_type = Attribute;
    using left_attribute_type = typename Left::attribute_type;
    using right_attribute_type = typename Right::attribute_type;

    static attribute_type& left(attribute_type& attr) {
        return attr;
    }

    static attribute_type& right(attribute_type& attr) {
        return attr;
    }

    template<typename Source>
    static void left_assign(attribute_type& attr, Source&& src) {
    }

    template<typename Source>
    static void right_assign(attribute_type& attr, Source&& src) {
    }
};

template<typename Left, typename Right, typename Attribute>
struct alternative_attribute_strategy<
    Left, Right, Attribute,
    std::enable_if_t<
        !std::is_same_v<Attribute, empty_attribute> &&
        !is_alternative_v<Left> && is_alternative_v<Right>
    >
> {
    using attribute_type = Attribute;
    using left_attribute_type = typename Left::attribute_type;
    using right_attribute_type = typename Right::attribute_type;

    static left_attribute_type left(attribute_type& attr) {
        return {};
    }

    static attribute_type& right(attribute_type& attr) {
        return attr;
    }

    template<typename Source>
    static void left_assign(attribute_type& attr, Source&& src) {
        assign(attr, std::forward<Source>(src));
    }

    template<typename Source>
    static void right_assign(attribute_type& attr, Source&& src) {
    }
};

template<typename Left, typename Right, typename Attribute>
struct alternative_attribute_strategy<
    Left, Right, Attribute,
    std::enable_if_t<
        !std::is_same_v<Attribute, empty_attribute> &&
        is_alternative_v<Left> && !is_alternative_v<Right>
    >
> {
    using attribute_type = Attribute;
    using left_attribute_type = typename Left::attribute_type;
    using right_attribute_type = typename Right::attribute_type;

    static attribute_type& left(attribute_type& attr) {
        return attr;
    }

    static right_attribute_type right(attribute_type& attr) {
        return {};
    }

    template<typename Source>
    static void left_assign(attribute_type& attr, Source&& src) {
    }

    template<typename Source>
    static void right_assign(attribute_type& attr, Source&& src) {
        assign(attr, std::forward<Source>(src));
    }
};

template<typename Left, typename Right, typename Attribute>
struct alternative_attribute_strategy<
    Left, Right, Attribute,
    std::enable_if_t<
        !std::is_same_v<Attribute, empty_attribute> &&
        !is_alternative_v<Left> && !is_alternative_v<Right>
    >
> {
    using attribute_type = Attribute;
    using left_attribute_type = typename Left::attribute_type;
    using right_attribute_type = typename Right::attribute_type;

    static left_attribute_type left(attribute_type& attr) {
        return {};
    }

    static right_attribute_type right(attribute_type& attr) {
        return {};
    }

    template<typename Source>
    static void left_assign(attribute_type& attr, Source&& src) {
        assign(attr, std::forward<Source>(src));
    }

    template<typename Source>
    static void right_assign(attribute_type& attr, Source&& src) {
        assign(attr, std::forward<Source>(src));
    }
};


/**
 * \brief Sequence Size
 *
 * Will consider any combination of nested sequences a part of the sequence
 */
template<typename T, typename = void>
struct sequence_size {
    //\TODO consider non-consuming parsers
    constexpr static size_t value = 1u;
};

template<typename Left_type, typename Right_type>
struct sequence_size<seq_parser<Left_type, Right_type>> {
    //\TODO consider non-consuming parsers
    constexpr static size_t value = sequence_size<Left_type>::value +
                                    sequence_size<Right_type>::value;
};

/**
 * \brief Sequence Size helper variable template
 */
template<typename T>
constexpr size_t sequence_size_v = sequence_size<T>::value;

/**
 * \brief Collect Binary Parser Types
 *
 * Helper template for reducing a binary tree of parsers to a flat tuple
 * structure
 */
template<typename T>
struct collect_binary_parser_types;

template<typename T>
struct collect_binary_parser_types {
    using type = decltype(std::make_tuple(std::declval<typename T::attribute_type>()));
};

template<typename Left, typename Right>
struct collect_binary_parser_types_helper{
    //use the return type of tuple_cat to concat the
    //result of collect_seq_types<...>::type recursively
    //results in a std::tuple
    using type = decltype(
        std::tuple_cat(
            std::declval<
                typename collect_binary_parser_types<
                    Left
                >::type
            >(),
            std::declval<
                typename collect_binary_parser_types<
                    Right
                >::type
            >()
        )
    );
};

template<typename Left, typename Right>
struct collect_binary_parser_types<seq_parser<Left, Right>>
    : collect_binary_parser_types_helper<Left, Right> {};

template<typename Left, typename Right>
struct collect_binary_parser_types<alt_parser<Left, Right>>
    : collect_binary_parser_types_helper<Left, Right> {};

/**
 * \brief Template variable helper
 */
template<typename T>
using collect_binary_parser_types_t = typename collect_binary_parser_types<T>::type;

template<typename>
struct is_sequence : std::false_type {};

template<typename Left, typename Right>
struct is_sequence<seq_parser<Left, Right>> : std::true_type {};

template<typename T>
constexpr bool is_sequence_v = is_sequence<T>::value;

/**
 * \brief Move Sequence Part helper class
 * guides seq_parser in slicing a tuple into a tuple_view recursively
 * until it matches the tree
 */
template<typename Left, typename Right, typename Attribute, typename = void>
struct sequence_attribute_strategy;

/**
 * \brief Move Seq Parser Attribute Strategy (Empty)
 */
template<typename Left, typename Right, typename Attribute>
struct sequence_attribute_strategy<
    Left, Right, Attribute,
    std::enable_if_t<
        std::is_same_v<Attribute, empty_attribute> &&
        !traits::is_vector_v<Attribute> &&
        !traits::is_tuple_v<Attribute> &&
        !is_sequence_v<Left> && !is_sequence_v<Right>
    >
> {

    using attribute_type = Attribute;

    static empty_attribute left(empty_attribute) { return {}; }
    static empty_attribute right(empty_attribute) { return {}; }

    template<typename Source>
    static void left_assign(empty_attribute, Source&& src) {}
    template<typename Source>
    static void right_assign(empty_attribute, Source&& src) {}
};

/**
 * \brief Move Seq Parser Attribute Strategy (Tuple)
 */
template<typename Left, typename Right, typename Attribute>
struct sequence_attribute_strategy<
    Left, Right, Attribute,
    std::enable_if_t<
        !std::is_same_v<Attribute, empty_attribute> &&
        !traits::is_vector_v<Attribute> &&
        traits::is_tuple_v<Attribute> &&
        is_sequence_v<Left> && is_sequence_v<Right>
    >
> {

    using attribute_type = Attribute;
    using parser_type = seq_parser<Left, Right>;
    using left_attribute_type = typename Left::attribute_type;
    using right_attribute_type = typename Right::attribute_type;

    constexpr static size_t left_size = std::tuple_size_v<left_attribute_type>;
    constexpr static size_t right_size = std::tuple_size_v<right_attribute_type>;

    static_assert(std::tuple_size_v<attribute_type> == (left_size + right_size), "Attribute size must match parser tree.");

    static auto left(attribute_type& attr) { return util::tuple_slice_view<0, left_size>(attr); }
    static auto right(attribute_type& attr) { return util::tuple_slice_view<left_size, right_size>(attr); }

    template<typename Source>
    static void left_assign(attribute_type& attr, Source&& src) {}
    template<typename Source>
    static void right_assign(attribute_type& attr, Source&& src) {}
};

/**
 * \brief Move Seq Parser Attribute Strategy (Tuple)
 */
template<typename Left, typename Right, typename Attribute>
struct sequence_attribute_strategy<
    Left,Right, Attribute,
    std::enable_if_t<
        !std::is_same_v<Attribute, empty_attribute> &&
        !traits::is_vector_v<Attribute> &&
        traits::is_tuple_v<Attribute> &&
        is_sequence_v<Left> && !is_sequence_v<Right>
    >
> {

    using attribute_type = Attribute;
    using parser_type = seq_parser<Left, Right>;
    using left_attribute_type = typename Left::attribute_type;
    using right_attribute_type = typename Right::attribute_type;

    constexpr static size_t left_size = std::tuple_size_v<left_attribute_type>;
    constexpr static size_t right_size = std::tuple_size_v<right_attribute_type>;

    static auto left(attribute_type& attr) { return util::tuple_slice_view<0, left_size>(attr); }
    static auto& right(attribute_type& attr) { return std::get<left_size>(attr); }

    template<typename Source>
    static void left_assign(attribute_type&, Source&&) {}
    template<typename Source>
    static void right_assign(attribute_type& attr, Source&& src) { assign(std::get<left_size>(attr), std::forward<Source>(src)); }
};


/**
 * \brief Move Seq Parser Attribute Strategy (Tuple)
 */
template<typename Left, typename Right, typename Attribute>
struct sequence_attribute_strategy<
    Left,Right, Attribute,
    std::enable_if_t<
        !std::is_same_v<Attribute, empty_attribute> &&
        !traits::is_vector_v<Attribute> &&
        traits::is_tuple_v<Attribute> &&
        !is_sequence_v<Left> && is_sequence_v<Right>
    >
> {

    using attribute_type = Attribute;
    using parser_type = seq_parser<Left, Right>;
    using left_attribute_type = typename Left::attribute_type;
    using right_attribute_type = typename Right::attribute_type;

    constexpr static size_t right_size = std::tuple_size_v<right_attribute_type>;

    static auto& left(attribute_type& attr) { return std::get<0>(attr); }
    static auto right(attribute_type& attr) { return util::tuple_slice_view<1, right_size>(attr); }

    template<typename Source>
    static void left_assign(attribute_type& attr, Source&& src) { assign(std::get<0>(attr), std::forward<Source>(src)); }
    template<typename Source>
    static void right_assign(attribute_type& attr, Source&& src) {}
};

/**
 * \brief Move Seq Parser Attribute Strategy (Tuple)
 */
template<typename Left, typename Right, typename Attribute>
struct sequence_attribute_strategy<
    Left,Right, Attribute,
    std::enable_if_t<
        !std::is_same_v<Attribute, empty_attribute> &&
        !traits::is_vector_v<Attribute> &&
        traits::is_tuple_v<Attribute> &&
        !is_sequence_v<Left> && !is_sequence_v<Right>
    >
> {

    using attribute_type = Attribute;
    using parser_type = seq_parser<Left, Right>;

    static auto& left(attribute_type& attr) { return std::get<0>(attr); }
    static auto& right(attribute_type& attr) { return std::get<1>(attr); }

    template<typename Source>
    static void left_assign(attribute_type& attr, Source&& src) { assign(std::get<0>(attr), std::forward<Source>(src)); }

    template<typename Source>
    static void right_assign(attribute_type& attr, Source&& src) { assign(std::get<1>(attr), std::forward<Source>(src)); }
};

/**
 * \brief Move Seq Parser Attribute Strategy (Vector)
 */
template<typename Left, typename Right, typename Attribute>
struct sequence_attribute_strategy<
    Left, Right, Attribute,
    std::enable_if_t<
        !std::is_same_v<Attribute, empty_attribute> &&
        traits::is_vector_v<Attribute> &&
        !traits::is_tuple_v<Attribute>
    >
> {

    using attribute_type = Attribute;

    static attribute_type left(attribute_type& attr) { return {}; }
    static attribute_type right(attribute_type& attr) { return {}; }

    static void left_assign(attribute_type& attr, attribute_type&& src) { assign(attr, src, container_attribute{});}
    static void right_assign(attribute_type& attr, attribute_type&& src) { assign(attr, src, container_attribute{}); }
};


/**
 * \brief Move Seq Parser Attribute Strategy (Vector)
 */
template<typename Left, typename Right, typename Attribute>
struct sequence_attribute_strategy<
    Left, Right, Attribute,
    std::enable_if_t<
        !std::is_same_v<Attribute, empty_attribute> &&
        !traits::is_vector_v<Attribute> &&
        !traits::is_tuple_v<Attribute>
    >
> {

    static_assert(
        !std::is_same_v<Attribute, empty_attribute> &&
        !traits::is_vector_v<Attribute> &&
        !traits::is_tuple_v<Attribute>,
        "Sequence attribute strategy is exhausted"
    );
};

}
SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_DETAIL_ATTRIBUTE_HPP */

