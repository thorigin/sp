/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#define BOOST_TEST_MODULE sp_alg_nn_optimizers

#include <boost/test/unit_test.hpp>
#include "sp/algo/nn/loss.hpp"
#include "assert_matrix.hpp"


using namespace sp::algo::nn;
using namespace sp::testing;

BOOST_AUTO_TEST_CASE(test_loss_mean_square) {
    mean_square_error mse;

    tensor_4 y(1, 2, 2, 2);
    tensor_4 x(1, 2, 2, 2);

    y.setValues({{{{1,2},{3,4}},{{5,6},{7,8}}}});
    x.setValues({{{{8,7},{6,5}},{{4,3},{2,1}}}});

    auto result = mse(0, y, x);
    auto expected = (
        (1-8)*(1-8) +
        (2-7)*(2-7) +
        (3-6)*(3-6) +
        (4-5)*(4-5) +
        (5-4)*(5-4) +
        (6-3)*(6-3) +
        (7-2)*(7-2) +
        (8-1)*(8-1)
    ) / 8.0f;

    BOOST_REQUIRE_EQUAL(result, expected);

}

BOOST_AUTO_TEST_CASE(test_nn_loss_mean_square_derivative) {

    mean_square_error mse;

    tensor_4 y(1, 2, 2, 2);
    tensor_4 x(1, 2, 2, 2);

    y.setValues({{{{1,2},{3,4}},{{5,6},{7,8}}}});
    x.setValues({{{{8,7},{6,5}},{{4,3},{2,1}}}});

    tensor_4 expected(1, 2, 2, 2);
    float_t f = (2.0f/8.0f);

    BOOST_CHECK_CLOSE(y(0, 0, 0, 0), 1, 0.001);
    BOOST_CHECK_CLOSE(x(0, 0, 0, 0), 8, 0.001);

    expected.setValues({{{
        {f*(y(0, 0, 0, 0) - x(0, 0, 0, 0)), f*(y(0, 0, 0, 1) - x(0, 0, 0, 1))},
        {f*(y(0, 0, 1, 0) - x(0, 0, 1, 0)), f*(y(0, 0, 1, 1) - x(0, 0, 1, 1))}
    },{
        {f*(y(0, 1, 0, 0) - x(0, 1, 0, 0)), f*(y(0, 1, 0, 1) - x(0, 1, 0, 1))},
        {f*(y(0, 1, 1, 0) - x(0, 1, 1, 0)), f*(y(0, 1, 1, 1) - x(0, 1, 1, 1))}
    }}});

    tensor_4 actual(1, 2, 2, 2);

    mse.derivative(0, y, x, actual);

    assert_tensor_equals(expected, actual);
}