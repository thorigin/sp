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

BOOST_AUTO_TEST_CASE(test_pooling_layer_mean_fprop_stride) {

    using input_dims = volume_dims<1, 4, 4>;
    using k_params = pooling_kernel_params<2, 2>;

    using mean_pooling_layer_type = mean_pooling_layer<input_dims, k_params>;

    mean_pooling_layer_type layer;

    layer.weight_initializer = fixed_weight_initializer(1);
    layer.bias_initializer = fixed_weight_initializer(0);
    layer.configure(1, true);

    /* Layer weight and bias override */
    tensor_4 input(1, 1, 4, 4);
    input.setValues({{{
        {0,  1,  2,  3},
        {8,  7,  5,  6},
        {4,  3,  1,  2},
        {0, -1, -2, -3}
    }}});

    tensor_4 result(1, 1, 2, 2);

    tensor_4 expected_result(1, 1, 2, 2);

    expected_result.setValues({{{
        { 4,    4  },
        { 1.5, -0.5},
    }}});

    layer.forward_prop(input, result);

    assert_tensor_equals(expected_result, result);

}

BOOST_AUTO_TEST_CASE(test_pooling_layer_mean_fprop_stride_with_weights_and_biases) {

    using input_dims = volume_dims<1, 4, 4>;
    using k_params = pooling_kernel_params<2, 2>;

    using mean_pooling_layer_type = mean_pooling_layer<input_dims, k_params>;

    mean_pooling_layer_type layer;

    layer.weight_initializer = vector_weight_initializer({1.25});
    layer.bias_initializer = vector_weight_initializer({0.5});
    layer.configure(1, true);

    /* Layer weight and bias override */
    tensor_4 input(1, 1, 4, 4);
    input.setValues({{{
        {-0.948148,  0.863082,   0.0993249, 0.895461},
        {-0.129355, -0.0305018, -0.159264, -0.358927},
        {-0.33933,  -0.691147,  -0.590703,  0.397725},
        {0.238542,  -0.760099,  -0.400691, -0.0296482}
    }}});

    tensor_4 result(1, 1, 2, 2);

    tensor_4 expected_result(1, 1, 2, 2);

    expected_result.setValues({{{
        { 0.423461625, 0.64893590625},
        { 0.014989375, 0.305213375}
    }}});

    layer.forward_prop(input, result);
    assert_tensor_equals(expected_result, result);

}

BOOST_AUTO_TEST_CASE(test_pooling_layer_mean_bprop_stride) {

    using input_dims = volume_dims<1, 4, 4>;
    using k_params = pooling_kernel_params<2, 2>;

    using avg_pooling_layer_type = mean_pooling_layer<input_dims, k_params>;

    avg_pooling_layer_type layer;

    layer.configure(1, true);

    /* Layer weight and bias override */
    tensor_4 prev_out(1, 1, 4, 4);
    tensor_4 curr_out(1, 1, 2, 2); curr_out.setZero();
    tensor_4 prev_delta(1, 1, 4, 4); prev_delta.setZero();
    tensor_4 expected_prev_delta(1, 1, 4, 4);
    tensor_4 curr_delta(1, 1, 2, 2);

    prev_out.setValues({{{
        {0,  1,  2,  3},
        {8,  7,  5,  6},
        {4,  3,  1,  2},
        {0, -1, -2, -3}
    }}});
    curr_delta.setValues({{{
        {1, 2},
        {3, 4},
    }}});
    expected_prev_delta.setValues({{{
        {1, 1, 2, 2},
        {1, 1, 2, 2},
        {3, 3, 4, 4},
        {3, 3, 4, 4}
    }}});
    expected_prev_delta = expected_prev_delta * (1 / 4.0f);

    tensor_4 result(1, 1, 2, 2);

    layer.forward_prop(prev_out, result);

    layer.backward_prop(prev_out, prev_delta, curr_out, curr_delta);

    assert_tensor_equals(expected_prev_delta, prev_delta);

}

BOOST_AUTO_TEST_CASE(test_pooling_layer_max_bprop_stride) {

    using input_dims = volume_dims<1, 4, 4>;
    using k_params = pooling_kernel_params<2, 2>;
    using max_pooling_layer_type = max_pooling_layer<input_dims, k_params>;

    max_pooling_layer_type layer;
    layer.weight_initializer = fixed_weight_initializer(1.0f);
    layer.configure(1, true);

    tensor_4 prev_out(1, 1, 4, 4);
    tensor_4 curr_out(1, 1, 2, 2); curr_out.setZero();
    tensor_4 prev_delta(1, 1, 4, 4); prev_delta.setZero();
    tensor_4 expected_prev_delta(1, 1, 4, 4);
    tensor_4 curr_delta(1, 1, 2, 2);

    prev_out.setValues({{{
        {0,  1,  2,  3},
        {8,  7,  5,  6},
        {4,  3,  1,  2},
        {0, -1, -2, -3}
    }}});
    curr_delta.setValues({{{
        {1, 2},
        {3, 4},
    }}});
    expected_prev_delta.setValues({{{
        {0, 0, 0, 0},
        {1, 0, 0, 2},
        {3, 0, 0, 4},
        {0, 0, 0, 0}
    }}});

    layer.forward_prop(prev_out, curr_out);


    layer.backward_prop(prev_out, prev_delta, curr_out, curr_delta);

    assert_tensor_equals(expected_prev_delta, prev_delta);

}
BOOST_AUTO_TEST_CASE(test_pooling_layer_mean_gradient_check) {

    /** FIX SEED FOR TESTING, REPRODUCIBLE RESULTS< NOT TO BE COMMITTED! */
    random_generator::get().seed(2);
    using input_dims = volume_dims<1, 4, 4>;
    using k_params = pooling_kernel_params<2, 2>;

    constexpr float_t epsilon = 1e-2f;
    constexpr size_t batch_size = 1;

    using avg_pooling_layer_type = mean_pooling_layer<input_dims, k_params>;
    avg_pooling_layer_type layer;

    layer.weight_initializer = gauss_weight_initializer(-1.0f, 1.0f);
    layer.bias_initializer = gauss_weight_initializer(-1.0f, 1.0f);

    /* setup weights and dimensions */
    layer.configure(batch_size, true);

    auto in = generate_inputs_for(layer, batch_size);

    for(size_t i = 0; i < 1; ++i) {
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
