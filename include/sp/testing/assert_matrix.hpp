/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_TESTING_ASSERT_MATRIX_HPP
#define	SP_ALGO_TESTING_ASSERT_MATRIX_HPP

#include <boost/assert.hpp>

#include "sp/config.hpp"
#include "sp/algo/nn/matrix.hpp"


/**
 * @file Matrix assertions utilities (tensor size 1-4)
 */

SP_TESTING_NAMESPACE_BEGIN

inline void assert_tensor_equals(const algo::nn::tensor_1& expected, const algo::nn::tensor_1& real, float epsilon = 1e-9) {
    BOOST_REQUIRE_EQUAL(expected.dimension(0), real.dimension(0));
    for(size_t i = 0, len = expected.dimension(2); i < len; ++i) {
        auto expect_val = expected(i);
        auto real_val = real(i);
        BOOST_REQUIRE_CLOSE(expect_val, real_val, epsilon);
    }
}

inline void assert_tensor_equals(const algo::nn::tensor_2& expected, const algo::nn::tensor_2& real, float epsilon = 1e-9) {
    BOOST_REQUIRE_EQUAL(expected.dimension(0), real.dimension(0));
    BOOST_REQUIRE_EQUAL(expected.dimension(1), real.dimension(1));
    for(size_t i = 0, i_len = expected.dimension(0); i < i_len; ++i) {
        for (size_t j = 0, j_len = expected.dimension(1); j < j_len; ++j) {
            auto expect_val = expected(i, j);
            auto real_val = real(i, j);
            BOOST_REQUIRE_CLOSE(expect_val, real_val, epsilon);
        }
    }
}

inline void assert_tensor_equals(const algo::nn::tensor_3& expected, const algo::nn::tensor_3& real, float epsilon = 1e-9) {
    BOOST_REQUIRE_EQUAL(expected.dimension(0), real.dimension(0));
    BOOST_REQUIRE_EQUAL(expected.dimension(1), real.dimension(1));
    BOOST_REQUIRE_EQUAL(expected.dimension(2), real.dimension(2));
    for(size_t i = 0, i_len = expected.dimension(0); i < i_len; ++i) {
        for (size_t j = 0, j_len = expected.dimension(1); j < j_len; ++j) {
            for(size_t k = 0, k_len = expected.dimension(2); k < k_len; ++k) {
                auto expect_val = expected(i, j, k);
                auto real_val = real(i, j, k);
                BOOST_REQUIRE_CLOSE(expect_val, real_val, epsilon);
            }
        }
    }
}

inline void assert_tensor_equals(const algo::nn::tensor_4& expected, const algo::nn::tensor_4& real, float epsilon = 1e-9) {
    BOOST_REQUIRE_EQUAL(expected.dimension(0), real.dimension(0));
    BOOST_REQUIRE_EQUAL(expected.dimension(1), real.dimension(1));
    BOOST_REQUIRE_EQUAL(expected.dimension(2), real.dimension(2));
    BOOST_REQUIRE_EQUAL(expected.dimension(3), real.dimension(3));
    for(size_t i = 0, i_len = expected.dimension(0); i < i_len; ++i) {
        for (size_t j = 0, j_len = expected.dimension(1); j < j_len; ++j) {
            for(size_t k = 0, k_len = expected.dimension(2); k < k_len; ++k) {
                for(size_t s = 0, s_len = expected.dimension(3); s < s_len; ++s) {
                    auto expect_val = expected(i, j, k, s);
                    auto real_val = real(i, j, k, s);
                    BOOST_REQUIRE_CLOSE(expect_val, real_val, epsilon);
                }
            }
        }
    }
}

inline void assert_tensor_equals(const algo::nn::tensor_5& expected, const algo::nn::tensor_5& real, float epsilon = 1e-9) {
    BOOST_REQUIRE_EQUAL(expected.dimension(0), real.dimension(0));
    BOOST_REQUIRE_EQUAL(expected.dimension(1), real.dimension(1));
    BOOST_REQUIRE_EQUAL(expected.dimension(2), real.dimension(2));
    BOOST_REQUIRE_EQUAL(expected.dimension(3), real.dimension(3));
    BOOST_REQUIRE_EQUAL(expected.dimension(4), real.dimension(4));
    for(size_t i = 0, i_len = expected.dimension(0); i < i_len; ++i) {
        for (size_t j = 0, j_len = expected.dimension(1); j < j_len; ++j) {
            for(size_t k = 0, k_len = expected.dimension(2); k < k_len; ++k) {
                for(size_t s = 0, s_len = expected.dimension(3); s < s_len; ++s) {
                    for(size_t r = 0, r_len = expected.dimension(4); r < r_len; ++r) {
                        auto expect_val = expected(i, j, k, s, r);
                        auto real_val = real(i, j, k, s, r);
                        BOOST_REQUIRE_CLOSE(expect_val, real_val, epsilon);
                    }
                }
            }
        }
    }
}

SP_TESTING_NAMESPACE_END

#endif	/* SP_ALGO_TESTING_ASSERT_MATRIX_HPP */

