/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#define BOOST_TEST_MODULE sp_util_tuples
#include <boost/test/unit_test.hpp>
#include <string>

#include <iostream>

#include "sp/util/typename.hpp"
#include "sp/util/tuples.hpp"

using namespace sp::util;

template<typename ... T>
using tupl = std::tuple<T...>;

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_slice_1_rvalue) {
    using input_type = tupl<bool, short, int, float, double>;
    using output_type = tupl<bool&&>;
    auto result = tuple_slice<0,1>(input_type{});
    BOOST_REQUIRE_EQUAL(std::tuple_size_v<decltype(result)>, 1);
    BOOST_REQUIRE((std::is_same_v<decltype(result), output_type>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_slice_1_lvalue) {
    using input_type = tupl<bool, short, int, float, double>;
    using output_type = tupl<bool&>;
    auto lvalue = input_type{};
    auto result = tuple_slice<0,1>(lvalue);
    BOOST_REQUIRE_EQUAL(std::tuple_size_v<decltype(result)>, 1);
    BOOST_REQUIRE((std::is_same_v<decltype(result), output_type>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_slice_2_rvalue) {
    using input_type = tupl<bool, short, int, float, double>;
    using output_type = tupl<short&&, int&&, float&&>;
    auto result = tuple_slice<1,4>(input_type{});
    BOOST_REQUIRE_EQUAL(std::tuple_size_v<decltype(result)>, 3);
    BOOST_REQUIRE((std::is_same_v<decltype(result), output_type>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_slice_2_lvalue) {
    using input_type = tupl<bool, short, int, float, double>;
    using output_type = tupl<short&, int&, float&>;
    auto lvalue = input_type{};
    auto result = tuple_slice<1,4>(lvalue);
    BOOST_REQUIRE_EQUAL(std::tuple_size_v<decltype(result)>, 3);
    BOOST_REQUIRE((std::is_same_v<decltype(result), output_type>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_slice_3_rvalue) {
    using input_type = tupl<bool, short, int, float, double>;
    using output_type = tupl<double&&>;
    auto result = tuple_slice<4,5>(input_type{});
    BOOST_REQUIRE_EQUAL(std::tuple_size_v<decltype(result)>, 1);
    BOOST_REQUIRE((std::is_same_v<decltype(result), output_type>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_slice_3_lvalue) {
    using input_type = tupl<bool, short, int, float, double>;
    using output_type = tupl<double&>;
    auto lvalue = input_type{};
    auto result = tuple_slice<4,5>(lvalue);
    BOOST_REQUIRE_EQUAL(std::tuple_size_v<decltype(result)>, 1);
    BOOST_REQUIRE((std::is_same_v<decltype(result), output_type>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_slice_rvalue_t) {
    using input_type = tupl<bool, short, int, float, double>;
    using output_type = tupl<double&&>;
    using result = tuple_slice_rvalue_t<4,5, input_type>;
    BOOST_REQUIRE_EQUAL(std::tuple_size_v<result>, 1);
    BOOST_REQUIRE((std::is_same_v<result, output_type>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_slice_lvalue_t) {
    using input_type = tupl<bool, short, int, float, double>;
    using output_type = tupl<double&>;
    using result = tuple_slice_lvalue_t<4,5, input_type>;
    BOOST_REQUIRE_EQUAL(std::tuple_size_v<result>, 1);
    BOOST_REQUIRE((std::is_same_v<result, output_type>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_slice_t) {
    using input_type = tupl<bool, short, int, float, double>;
    using output_type = tupl<double&>;
    using result = tuple_slice_t<4,5, input_type>;
    BOOST_REQUIRE_EQUAL(std::tuple_size_v<result>, 1);
    BOOST_REQUIRE((std::is_same_v<result, output_type>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_head) {
    using input_type = tupl<bool, short, int, float, double>;
    using expected = bool;
    using result = tuple_head_t<input_type>;
    BOOST_REQUIRE((std::is_same_v<result, expected>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_tail) {
    using input_type = tupl<bool, short, int, float, double>;
    using expected = double;
    using result = tuple_tail_t<input_type>;
    BOOST_REQUIRE((std::is_same_v<result, expected>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_pop_front) {
    using input_type = tupl<bool, short, int, float, double>;
    using expected = tupl<short, int, float, double>;
    using result = tuple_pop_front_t<input_type>;
    BOOST_REQUIRE((std::is_same_v<result, expected>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_pop) {
    using input_type = tupl<bool, short, int, float, double>;
    using expected = tupl<short, int, float, double>;
    using result = tuple_pop_t<input_type>;
    BOOST_REQUIRE((std::is_same_v<result, expected>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_pop_end) {
    using input_type = tupl<bool, short, int, float, double>;
    using expected = tupl<bool, short, int, float>;
    using result = tuple_pop_end_t<input_type>;
    BOOST_REQUIRE((std::is_same_v<result, expected>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_push_back) {
    using adding = long long;
    using input_type = tupl<bool, short, int, float, double>;
    using expected = tupl<bool, short, int, float, double, adding>;
    using result = tuple_push_back_t<input_type, adding>;
    BOOST_REQUIRE((std::is_same_v<result, expected>));
}


BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_push) {
    using adding = long long;
    using input_type = tupl<bool, short, int, float, double>;
    using expected = tupl<bool, short, int, float, double, adding>;
    using result = tuple_push_t<input_type, adding>;
    BOOST_REQUIRE((std::is_same_v<result, expected>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_push_front) {
    using adding = long long;
    using input_type = tupl<bool, short, int, float, double>;
    using expected = tupl<adding, bool, short, int, float, double>;
    using result = tuple_push_front_t<input_type, adding>;
    BOOST_REQUIRE((std::is_same_v<result, expected>));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_view_from_tuple_1) {
    using input_type = tupl<bool, short, int, float, double>;
    using expected = tuple_view<1, 4, input_type>;
    auto input = input_type{true, 1, 2, 3, 4};
    auto result = tuple_slice_view<1, 4>(input);
    BOOST_REQUIRE((std::is_same_v<decltype(result), expected>));
    BOOST_REQUIRE_EQUAL(tuple_view_size_v<decltype(result)>, 3);
    BOOST_REQUIRE_EQUAL(std::get<1>(input), std::get<0>(result));
    BOOST_REQUIRE_EQUAL(std::get<2>(input), std::get<1>(result));
    BOOST_REQUIRE_EQUAL(std::get<3>(input), std::get<2>(result));
}

    
BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_view_from_tuple_2) {
    using input_type = tupl<bool, short, int, float, double>;
    using expected = tuple_view<0, 3, input_type>;
    auto input = input_type{true, 1, 2, 3, 4};
    auto result = tuple_slice_view<0, 3>(input);
    BOOST_REQUIRE((std::is_same_v<decltype(result), expected>));
    BOOST_REQUIRE_EQUAL(tuple_view_size_v<decltype(result)>, 3);
    BOOST_REQUIRE_EQUAL(std::get<0>(input), std::get<0>(result));
    BOOST_REQUIRE_EQUAL(std::get<1>(input), std::get<1>(result));
    BOOST_REQUIRE_EQUAL(std::get<2>(input), std::get<2>(result));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_view_from_tuple_3) {
    using input_type = tupl<bool, short, int, float, double>;
    using expected = tuple_view<2, 5, input_type>;
    auto input = input_type{true, 1, 2, 3, 4};
    auto result = tuple_slice_view<2, 5>(input);
    BOOST_REQUIRE((std::is_same_v<decltype(result), expected>));
    BOOST_REQUIRE_EQUAL(tuple_view_size_v<decltype(result)>, 3);
    BOOST_REQUIRE_EQUAL(std::get<2>(input), std::get<0>(result));
    BOOST_REQUIRE_EQUAL(std::get<3>(input), std::get<1>(result));
    BOOST_REQUIRE_EQUAL(std::get<4>(input), std::get<2>(result));
}

BOOST_AUTO_TEST_CASE( test_sp_util_tuples_tuple_view_from_tuple_view_1) {
    using input_type = tupl<bool, short, int, float, double>;
    using expected = tuple_view<0, 5, input_type>;
    auto input_tuple = input_type{true, 1, 2, 3, 4};
    auto result_view = tuple_slice_view<0, 5>(input_tuple);
    BOOST_REQUIRE((std::is_same_v<decltype(result_view), expected>));
    BOOST_REQUIRE_EQUAL(std::get<0>(input_tuple), std::get<0>(result_view));
    BOOST_REQUIRE_EQUAL(std::get<1>(input_tuple), std::get<1>(result_view));
    BOOST_REQUIRE_EQUAL(std::get<2>(input_tuple), std::get<2>(result_view));
    BOOST_REQUIRE_EQUAL(std::get<3>(input_tuple), std::get<3>(result_view));
    BOOST_REQUIRE_EQUAL(std::get<4>(input_tuple), std::get<4>(result_view));
    auto result_view_child = tuple_slice_view<0,3>(result_view);   
    BOOST_REQUIRE_EQUAL(std::get<0>(input_tuple), std::get<0>(result_view_child));
    BOOST_REQUIRE_EQUAL(std::get<1>(input_tuple), std::get<1>(result_view_child));
    BOOST_REQUIRE_EQUAL(std::get<2>(input_tuple), std::get<2>(result_view_child));
    auto result_view_child_2 = tuple_slice_view<2,3>(result_view);   
    BOOST_REQUIRE_EQUAL(std::get<2>(input_tuple), std::get<0>(result_view_child_2)); 
}


BOOST_AUTO_TEST_CASE( test_sp_util_tuple_view_conserves_modifications) {
    using input_type = tupl<bool, short, int, float, double>;
    using expected = tuple_view<0, 5, input_type>;
    auto value = 999;
    auto input_tuple = input_type{true, 1, 2, 3, 4};
    auto result_view = tuple_slice_view<0, 5>(input_tuple);
    BOOST_REQUIRE((std::is_same_v<decltype(result_view), expected>));
    auto result_view_child = tuple_slice_view<0,3>(result_view);   
    auto result_view_child_2 = tuple_slice_view<2,3>(result_view);
    
    std::get<0>(result_view_child_2) = value;
    BOOST_REQUIRE_EQUAL(std::get<0>(result_view_child_2), value); 
    BOOST_REQUIRE_EQUAL(std::get<2>(result_view_child), value); 
    BOOST_REQUIRE_EQUAL(std::get<2>(input_tuple), value);     
}

















