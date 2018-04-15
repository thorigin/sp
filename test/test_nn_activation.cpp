/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

//#include "sp/util/timing.hpp"
//#include "sp/util/rand.hpp"
//#include "sp/util/typename.hpp"

#include <iostream>
#include <iomanip>
#define BOOST_TEST_MODULE sp_algo_nn
#include <boost/test/unit_test.hpp>
#include "sp/algo/nn.hpp"
#include "sp/algo/nn/gradient_check.hpp"
#include "assert_matrix.hpp"

using namespace sp::algo::nn;
using namespace sp::testing;

BOOST_AUTO_TEST_CASE(test_activation_tanh_fprop_bprop) {

    using tanh_act_layer = tanh_layer<volume_dims<1, 3, 3>>;
    tanh_act_layer layer;

    layer.configure(1, true);

    /* stage 2 */
    tensor_4 in = generate_inputs_for(layer);
    tensor_4 out(1, 1, 3, 3);

    tensor_4 prev_delta(1, 1, 3, 3);
    tensor_4 out_delta(1, 1, 3, 3);
    tensor_4 expected_out(1, 1, 3, 3);
    tensor_4 expected_prev_delta(1, 1, 3, 3);

    in.setValues({{{
        {0.0f, 1.0f, 2.0f},
        {1.0f, 2.0f, 3.0f},
        {2.0f, 3.0f, 4.0f},
    }}});
    prev_delta.setZero();
    out_delta.setValues({{{
        {-1.0f, 2.0f, 3.0f},
        { 4.0f, 5.0f, 6.0f},
        { 7.0f, 8.0f, 9.0f}
    }}});

    /* perform activation forward propagation*/
    layer.forward_prop(in, out);

    BOOST_REQUIRE((std::is_same_v<tanh_act_layer::activation_op_type, tanh_activation_op>));
    auto op = tanh_act_layer::activation_op_type();
    auto dop = tanh_act_layer::activation_op_deriv_type();
    expected_out.setValues({{{ //Simply performs acti(x) for every value in input
        {op(in(0, 0, 0, 0)), op(in(0, 0, 0, 1)), op(in(0, 0, 0, 2))},
        {op(in(0, 0, 1, 0)), op(in(0, 0, 1, 1)), op(in(0, 0, 1, 2))},
        {op(in(0, 0, 2, 0)), op(in(0, 0, 2, 1)), op(in(0, 0, 2, 2))},
    }}});

    assert_tensor_equals(expected_out, out);

    layer.backward_prop(in, prev_delta, out, out_delta);

    expected_prev_delta.setValues({{{ //act_delta(x, y, ...) = prev_delta(x, y, ...) * deriv_op(prev_delta(x, y, ...))
        {out_delta(0, 0, 0, 0) * dop(out(0, 0, 0, 0)), out_delta(0, 0, 0, 1) * dop(out(0, 0, 0, 1)), out_delta(0, 0, 0, 2) * dop(out(0, 0, 0, 2))},
        {out_delta(0, 0, 1, 0) * dop(out(0, 0, 1, 0)), out_delta(0, 0, 1, 1) * dop(out(0, 0, 1, 1)), out_delta(0, 0, 1, 2) * dop(out(0, 0, 1, 2))},
        {out_delta(0, 0, 2, 0) * dop(out(0, 0, 2, 0)), out_delta(0, 0, 2, 1) * dop(out(0, 0, 2, 1)), out_delta(0, 0, 2, 2) * dop(out(0, 0, 2, 2))},
    }}});

    assert_tensor_equals(expected_prev_delta, prev_delta);
}
BOOST_AUTO_TEST_CASE(test_activation_layer_grad_check) {
    using layer_type = tanh_layer<volume_dims<10, 3, 3>>;

    constexpr float_t epsilon = 1e-2f;
    constexpr size_t batch_size = 1;
    layer_type layer;

    layer.weight_initializer = gauss_weight_initializer(-1.0f, 1.0f);
    layer.bias_initializer = gauss_weight_initializer(-1.0f, 1.0f);

    /* setup weights and dimensions */
    layer.configure(batch_size, true);

    auto in = generate_inputs_for(layer, batch_size);

    for(size_t i = 0; i < 100; ++i) {
        auto in_selected  = gradient_random_input(layer);
        auto out_selected = gradient_random_output(layer);

        /* Perform numerical and analytics gradient */
        /* estimate */
        auto n = numerical_gradient (layer, in, in_selected, out_selected);
        /* actual */
        auto a = analytical_gradient(layer, in, in_selected, out_selected);

        /* Validate result */
        BOOST_CHECK_MESSAGE(std::abs(a-n) <= epsilon, "Gradient check |" << std::setprecision(15) << a << " - " << n << "| < " << epsilon);
    }
}
