/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#define BOOST_TEST_MODULE sp_alg_nn_optimizers

#include <boost/test/unit_test.hpp>
#include "sp/algo/nn/optimizer.hpp"
#include "sp/algo/nn/types.hpp"
#include "assert_matrix.hpp"

using namespace sp::algo::nn;
using namespace sp::testing;

using testing_learning_rate = std::ratio<1, 100>;

BOOST_AUTO_TEST_CASE(test_nn_optimizer_adaptive_gradient) {
    ada_gradient_optimizer<testing_learning_rate> optimizer;
    tensor_1 w(5); w.setValues({0.20, 0.40, 0.006, -0.77, -0.010});
    tensor_1 dw(5); dw.setValues({1.00, -3.24, -0.600, 2.79, 1.820});
    tensor_1 res1(5); res1.setValues({0.1900, 0.4100, 0.0160, -0.7800, -0.0200});
    tensor_1 res2(5); res2.setValues({0.1829, 0.4170, 0.0230, -0.7870, -0.0270});
    optimizer.update(dw, w);
    assert_tensor_equals(res1, w, 0.999);
    optimizer.update(dw, w);
    assert_tensor_equals(res2, w, 0.999);
}