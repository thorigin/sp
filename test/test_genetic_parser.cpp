/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#include <iostream>
#include "sp/algo/stats.hpp"
#include "sp/algo/gen.hpp"
#include "sp/algo/gen/parser.hpp"
#include "sp/util/units.hpp"

#include "sp/util/timing.hpp"
#include "sp/util/query.hpp"
#include "sp/util/typename.hpp"

#define BOOST_TEST_MODULE sp_algo_gen_parser
#include <boost/test/unit_test.hpp>
using namespace sp::algo::gen;
using namespace sp::algo;
using namespace sp::util;

BOOST_AUTO_TEST_CASE( parser_detail_sequence_collect_types_produces_flat_structure) {
    using namespace parser;
    auto result = dummy() >> dummy() >> dummy() >> dummy() >> dummy();
    size_t seq_len = ::parser::detail::sequence_size_v<decltype(result)>;
    BOOST_REQUIRE_EQUAL(seq_len, 5);
    using result_tuple_expected_type = parser::detail::collect_binary_parser_types_t<decltype(result)>;
    BOOST_REQUIRE((std::is_same_v<std::tuple<dv, dv, dv, dv, dv>, result_tuple_expected_type>));
}

BOOST_AUTO_TEST_CASE( parser_seq_parser_detail_sequence_attribute_strategy_size_3) {

    using reader_type = chromosome_bit_reader;
    chromosome ch{};
    reader_type reader(&ch);

    using namespace parser;

    auto left1 = dummy(1) >> dummy(2);
    auto right1 = dummy(3);

    std::tuple<dv, dv, dv> t;
    using msp_type = parser::detail::sequence_attribute_strategy<decltype(left1), decltype(right1), decltype(t)>;

    auto lattr1 = msp_type::left(t);
    auto rattr1 = msp_type::right(t);

    BOOST_REQUIRE(left1.parse(reader, lattr1));
    BOOST_REQUIRE(right1.parse(reader, rattr1));

    msp_type::left_assign(t, lattr1);
    msp_type::right_assign(t, rattr1);

    BOOST_REQUIRE_EQUAL(std::get<0>(t).value, 1);
    BOOST_REQUIRE_EQUAL(std::get<1>(t).value, 2);
    BOOST_REQUIRE_EQUAL(std::get<2>(t).value, 3);
}

BOOST_AUTO_TEST_CASE( parser_sequence_flatten_into_tuple_of_2) {

    using reader_type = chromosome_bit_reader;
    chromosome ch{};
    reader_type reader(&ch);

    using namespace parser;

    std::tuple<dv, dv, dv, dv> t;
    auto expr = dummy(4) >> dummy(3) >> dummy(2) >> dummy(1);
    BOOST_REQUIRE(expr.parse(reader, t));
    BOOST_REQUIRE_EQUAL(std::get<0>(t).value, 4);
    BOOST_REQUIRE_EQUAL(std::get<1>(t).value, 3);
    BOOST_REQUIRE_EQUAL(std::get<2>(t).value, 2);
    BOOST_REQUIRE_EQUAL(std::get<3>(t).value, 1);
}


BOOST_AUTO_TEST_CASE( parse_seq_with_two_literals ) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,1,0,1,1, 0,0,0,0};

    reader_type reader(&ch);

    auto expr = (lit<0, 1, 0, 1, 1>() >> lit<0, 0, 0, 0>());

    BOOST_REQUIRE(expr.parse(reader));
    BOOST_REQUIRE_EQUAL(reader.pos(), ch.size());

    reader.reset();

    ch[8] = 1;

    BOOST_REQUIRE(!expr.parse(reader));
    BOOST_REQUIRE_EQUAL(reader.pos(), 0);

}

BOOST_AUTO_TEST_CASE( parse_action_return_false_fails_valid_expression ) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,1,0,1,1,0,0,0,0};

    reader_type reader(&ch);

    auto expr = lit<0, 1, 0, 1, 1>() >> lit<0, 0, 0, 0>();
    auto fail_action = [&](auto& ctx) { return false; };

    BOOST_REQUIRE(expr.parse(reader));
    BOOST_REQUIRE_EQUAL(reader.pos(), ch.size());
    auto act_expr = expr[fail_action];
    reader.reset();
    BOOST_REQUIRE(!act_expr.parse(reader));

    reader.reset();

    ch[8] = 1;

    BOOST_REQUIRE(!expr.parse(reader));
    BOOST_REQUIRE_EQUAL(reader.pos(), 0);

}

BOOST_AUTO_TEST_CASE( parse_action_operator_return_false_fails_valid_expression ) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,1,0,1,1,0,0,0,0};

    reader_type reader(&ch);

    auto expr = lit<0, 1, 0, 1, 1>() >> lit<0, 0, 0, 0>();
    auto fail_action = [&](auto& ctx) { return false; };

    BOOST_REQUIRE(expr.parse(reader));
    BOOST_REQUIRE_EQUAL(reader.pos(), ch.size());
    auto act_expr = expr / fail_action;
    reader.reset();
    BOOST_REQUIRE(!act_expr.parse(reader));

    reader.reset();

    ch[8] = 1;

    BOOST_REQUIRE(!expr.parse(reader));
    BOOST_REQUIRE_EQUAL(reader.pos(), 0);

}

BOOST_AUTO_TEST_CASE( parse_literal_with_action_is_called ) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,1,0,1,1,0,0,0,0};

    reader_type reader(&ch);
    int action_counter = 0;
    auto test_action = [&](auto& ctx) { ++action_counter; };
    auto expr = (lit<0, 1, 0, 1, 1>()[test_action] >> lit<0, 0, 0, 0>()[test_action]);

    BOOST_REQUIRE(expr.parse(reader));
    BOOST_REQUIRE_EQUAL(reader.pos(), ch.size());
    BOOST_REQUIRE_EQUAL(action_counter, 2);

}

BOOST_AUTO_TEST_CASE( parser_seq_adapts_to_vector ) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,1,0,1,1,0,0,0,0};

    reader_type reader(&ch);

    auto expr = (lit<0, 1, 0, 1, 1>() >> lit<0, 0 , 0, 0>());

    std::vector<int> bit_sequences;
    BOOST_REQUIRE(expr.parse(reader, bit_sequences));
    BOOST_REQUIRE_EQUAL(reader.pos(), ch.size());
    BOOST_REQUIRE_EQUAL(bit_sequences.size(), 2);
}

BOOST_AUTO_TEST_CASE( parser_seq_attribute_not_added_on_failure) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,1,0,1,1, 0,0,0,0, 0,0,0,1};

    reader_type reader(&ch);

    auto expr = (lit<0, 1, 0, 1, 1>() >> lit<0, 0, 0, 1>() >> lit<0, 0, 0, 1>());

    std::vector<int> bit_sequences;
    BOOST_REQUIRE(!expr.parse(reader, bit_sequences));
    BOOST_REQUIRE_EQUAL(reader.pos(), 0);
    BOOST_REQUIRE_EQUAL(bit_sequences.size(), 0);
}

BOOST_AUTO_TEST_CASE( parser_seq_adapts_to_binary_tuple) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,1,0,1,1, 1,0,1,0};

    reader_type reader(&ch);

    auto expr = lit<0, 1, 0, 1, 1>() >> lit<1, 0, 1, 0>();

    std::tuple<int, int> bit_seqs;
    BOOST_REQUIRE(expr.parse(reader, bit_seqs));
    BOOST_REQUIRE_EQUAL(reader.pos(), ch.size());

    BOOST_REQUIRE_EQUAL(std::get<0>(bit_seqs), 11);
    BOOST_REQUIRE_EQUAL(std::get<1>(bit_seqs), 10);
}

BOOST_AUTO_TEST_CASE( parser_seq_adapts_to_tuple_of_three) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,1,0,1,1, 1,0,1,0, 0,1,0,1};

    reader_type reader(&ch);

    auto expr = (lit<0, 1, 0, 1, 1>() >> lit<1, 0, 1, 0>() >> lit<0, 1, 0, 1>());

    std::tuple<unsigned char,unsigned char,unsigned char> bit_seq;
    BOOST_REQUIRE(expr.parse(reader, bit_seq));
    BOOST_REQUIRE_EQUAL(reader.pos(), ch.size());

    BOOST_REQUIRE_EQUAL(std::get<0>(bit_seq), 11);

    BOOST_REQUIRE_EQUAL(std::get<1>(bit_seq), 10);

    BOOST_REQUIRE_EQUAL(std::get<2>(bit_seq), 5);
}

BOOST_AUTO_TEST_CASE( parser_seq_adapts_to_tuple_of_four) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,1,0,1,1, 1,0,1,0, 0,0,1,0,  0,0,0,1};

    reader_type reader(&ch);

    auto expr = lit<0,1,0,1,1>() >> lit<1,0,1,0>() >> lit<0,0,1,0>() >> lit<0,0,0,1>();

    std::tuple<int, int, int, int> bit_sequences;

    BOOST_REQUIRE(expr.parse(reader, bit_sequences));
    BOOST_REQUIRE_EQUAL(reader.pos(), ch.size());


    BOOST_REQUIRE_EQUAL(std::get<0>(bit_sequences), 11);

    BOOST_REQUIRE_EQUAL(std::get<1>(bit_sequences), 10);

    BOOST_REQUIRE_EQUAL(std::get<2>(bit_sequences), 2);

    BOOST_REQUIRE_EQUAL(std::get<3>(bit_sequences), 1);
}

BOOST_AUTO_TEST_CASE( parser_seq_adapts_to_vector_of_three) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,1,0,1,1, 1,0,1,0, 0, 1, 0, 1};

    reader_type reader(&ch);

    auto expr = (lit<0, 1, 0, 1, 1>() >> lit<1, 0, 1, 0>() >> lit<0, 1, 0, 1>());

    std::vector<int> bit_vec;
    BOOST_REQUIRE(expr.parse(reader, bit_vec));
    BOOST_REQUIRE_EQUAL(reader.pos(), ch.size());

    BOOST_REQUIRE_EQUAL(bit_vec.size(), 3);
    BOOST_REQUIRE_EQUAL(bit_vec[0], 11);
    BOOST_REQUIRE_EQUAL(bit_vec[1], 10);
    BOOST_REQUIRE_EQUAL(bit_vec[2], 5);
}

BOOST_AUTO_TEST_CASE( parser_alt_adapts_to_variant_of_four) {
    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,0,0,0,0,0,0,1};

    reader_type reader(&ch);

    auto expr = uint64_val(0) | uint32_val(0) | uint16_val(0) | uint8_val(1);
    std::variant<std::monostate, uint64_t, uint16_t, uint8_t, uint32_t> var;
    BOOST_REQUIRE(expr.parse(reader, var));
    BOOST_REQUIRE_EQUAL(reader.pos(), ch.size());
    BOOST_REQUIRE_EQUAL(std::get<uint8_t>(var), uint8_t(1));
}

BOOST_AUTO_TEST_CASE( parser_kleene_adapts_to_vector) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{1,0,1,0,1,0,1,0};

    reader_type reader(&ch);

    auto expr = *lit<1,0>();
    std::vector<int> vec;
    BOOST_REQUIRE(expr.parse(reader, vec));
    BOOST_REQUIRE_EQUAL(reader.pos(), ch.size());
    BOOST_REQUIRE_EQUAL(vec.size(), 4);
}

BOOST_AUTO_TEST_CASE( parser_not) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{1,0};

    reader_type reader(&ch);
    auto expr = !lit<1,0>();
    BOOST_REQUIRE(!expr.parse(reader));
    BOOST_REQUIRE_EQUAL(reader.pos(), 0);
}

BOOST_AUTO_TEST_CASE( parser_plus_and_adapts_to_vector) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{1,0, 1,0, 1,1};

    reader_type reader(&ch);
    auto expr = +lit<1,0>();
    std::vector<int> vec;
    BOOST_REQUIRE(expr.parse(reader, vec));
    BOOST_REQUIRE_EQUAL(reader.pos(), 4);
    BOOST_REQUIRE_EQUAL(vec.size(), 2);
    BOOST_REQUIRE_EQUAL(vec[0], 2);
    BOOST_REQUIRE_EQUAL(vec[1], 2);
}

BOOST_AUTO_TEST_CASE( parser_optional_adapts_to_std_optional_tuple) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{1,0, 1,1};

    reader_type reader(&ch);

    auto expr = (-lit<1,0>()) >> (-lit<1,0>());


    std::tuple<std::optional<int>, std::optional<int>> opt;

    BOOST_REQUIRE(expr.parse(reader, opt));
    BOOST_REQUIRE_EQUAL(reader.pos(), 2);
    BOOST_REQUIRE(std::get<0>(opt).has_value());
    BOOST_REQUIRE(!std::get<1>(opt).has_value());
}

BOOST_AUTO_TEST_CASE( parser_composite_tuple_of_sequences_and_alternatives_is_adapted_into_variant) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,1,1,1};

    reader_type reader(&ch);

    auto seq = bool_val(true) >> bool_any() >> bool_any() >> bool_val(false);
    auto seq2 = uint8_any<4>();

    auto expr = seq | seq2;

    std::variant<std::tuple<bool, bool, bool, bool>, int> var;

    BOOST_REQUIRE(expr.parse(reader, var));

    BOOST_REQUIRE_EQUAL(reader.pos(), 4);
    BOOST_REQUIRE(std::holds_alternative<int>(var));

    BOOST_REQUIRE_EQUAL(std::get<1>(var), 7);
}

BOOST_AUTO_TEST_CASE( parser_composite_tuple_of_multiple_sequences_and_alternatives_is_adapted_into_kleene_variant) {

    using namespace parser;
    using reader_type = chromosome_bit_reader;
    chromosome ch{0,1,1,1, 1,1,1,0};

    reader_type reader(&ch);

    auto seq = bool_val(true) >> bool_any() >> bool_any() >> bool_val(false);
    auto seq2 = uint8_any<4>();
    auto expr = +(seq | seq2);

    std::vector<
        std::variant<
            std::tuple<bool, bool, bool, bool>,
            int
        >
    > var;

    BOOST_REQUIRE(expr.parse(reader, var));
    BOOST_REQUIRE_EQUAL(reader.pos(), 8);

    BOOST_REQUIRE_EQUAL(var.size(), 2);
    BOOST_REQUIRE(std::holds_alternative<int>(var[0]));
    BOOST_REQUIRE((std::holds_alternative<std::tuple<bool, bool, bool, bool>>(var[1])));
    BOOST_REQUIRE_EQUAL(std::get<1>(var[0]), 7);
    BOOST_REQUIRE(std::get<0>(var[1]) == std::make_tuple(1,1,1,0));
}

BOOST_AUTO_TEST_CASE( parser_composite_tuple_of_sequences_and_alternatives_is_adapted_into_variant_with_explict_rules) {

    using namespace parser;
    using r_t = chromosome_bit_reader;
    chromosome ch{0,1,1,1};

    r_t reader(&ch);

    using t4 =  std::tuple<bool, bool, bool, bool>;
    rule<r_t, t4> seq;
    rule<r_t, int> seq2;
    rule<r_t, std::variant<std::monostate, t4, int>> expr;

    seq     %= bool_val(true) >> bool_any() >> bool_any() >> bool_val(false);
    seq2    %= uint8_any<4>();
    expr    %= seq | seq2;

    std::variant<std::monostate, std::tuple<bool, bool, bool, bool>, int> var;

    BOOST_REQUIRE(expr.parse(reader, var));

    BOOST_REQUIRE_EQUAL(reader.pos(), 4);
    BOOST_REQUIRE((!std::holds_alternative<std::tuple<bool, bool, bool, bool>>(var)));
    BOOST_REQUIRE(std::holds_alternative<int>(var));

    BOOST_REQUIRE_EQUAL(std::get<2>(var), 7);
}
