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
#include <boost/test/included/unit_test.hpp>
#include "sp/algo/nn.hpp"
#include "sp/testing/assert_matrix.hpp"

using namespace sp::algo::nn;
using namespace sp::testing;

//using rand_type = sp::util::rand_util<>;
//rand_type rand_inst;

////BOOST_AUTO_TEST_CASE(test_fcl_1_input_2x2_expect) {
////
////    fcl<1> f;
////
////    f.weight.fill(2);
////
////    tensor_4 i(1, 1, 2, 2);
////    //    i.setConstant(1);
////    i.setValues({//batch
////        { //Depth
////            { // H x W
////                {3, 4},
////                {5, 6}
////            }
////        }
////    }
////    );
////    auto res = f.feed_fwd(i);
////    //    cout << res.eval() << endl;
////    //BOOST_REQUIRE()
////}
////
//BOOST_AUTO_TEST_CASE(test_conv_layer_construct) {
//
////    using input = volume_dims<28, 28, 1>;
////    using cl = conv_layer<input, kernel_params<128>>;
////
////
////    cl cl1;
//}


BOOST_AUTO_TEST_SUITE( test_conv )

BOOST_AUTO_TEST_CASE(test_conv_out_params_detail_krizhevsky_et_al) {
    /**
     * Test output calculations for krizhevsky et al neural network (2012) output sizes
     */
    using input_dims = volume_dims<3, 227, 227>;
    using k_params = kernel_symmetric_params<96, 11, 4>;
    using out_dim = detail::convolution_kernel_out_dims_t<input_dims, k_params>;

    using conv_layer = conv_layer<input_dims, k_params>;

    /**
     * Validate input dimensions
     */
    BOOST_REQUIRE_EQUAL(input_dims::d, 3);
    BOOST_REQUIRE_EQUAL(input_dims::w, 227);
    BOOST_REQUIRE_EQUAL(input_dims::h, 227);

    /**
     * Validate conv_layer input dims
     */
    BOOST_REQUIRE_EQUAL(conv_layer::input_dims::d, 3);
    BOOST_REQUIRE_EQUAL(conv_layer::input_dims::w, 227);
    BOOST_REQUIRE_EQUAL(conv_layer::input_dims::h, 227);

    /**
     * Validate kernel parameters
     */
    BOOST_REQUIRE_EQUAL(k_params::d, 96);
    BOOST_REQUIRE_EQUAL(k_params::w, 11);
    BOOST_REQUIRE_EQUAL(k_params::h, 11);
    BOOST_REQUIRE_EQUAL(k_params::s_w, 4);
    BOOST_REQUIRE_EQUAL(k_params::s_h, 4);
    BOOST_REQUIRE_EQUAL(k_params::padding, padding_type::valid);

    /**
     * Validate output dimension
     */
    BOOST_REQUIRE_EQUAL(out_dim::w, 55);
    BOOST_REQUIRE_EQUAL(out_dim::h, 55);
    BOOST_REQUIRE_EQUAL(out_dim::d, 96);

    /**
     * Validate conv layer output dims
     */
    BOOST_REQUIRE_EQUAL(conv_layer::output_dims::w, 55);
    BOOST_REQUIRE_EQUAL(conv_layer::output_dims::h, 55);
    BOOST_REQUIRE_EQUAL(conv_layer::output_dims::d, 96);
}

BOOST_AUTO_TEST_CASE(test_conv_layer_initialization_krizhevsky_et_al) {
    /**
     * Test initialization of krizhevsky et all convolution layer (2d convo
     * with the specified input/output parameters
     */
    using input_dims = volume_dims<3, 227, 227>;
    using k_params = kernel_symmetric_params<96, 11, 4>;

    using conv_layer_type = conv_layer<input_dims, k_params>;
    conv_layer_type cl;

    /**
     * Validate weight dimensions
     */
    BOOST_REQUIRE_EQUAL(cl.w.dimension(0), conv_layer_type::kernel_params::d);
    BOOST_REQUIRE_EQUAL(cl.w.dimension(1), conv_layer_type::input_dims::d);
    BOOST_REQUIRE_EQUAL(cl.w.dimension(2), conv_layer_type::kernel_params::h);
    BOOST_REQUIRE_EQUAL(cl.w.dimension(3), conv_layer_type::kernel_params::w);

    /**
     * Validate bias dimension
     */
    BOOST_REQUIRE_EQUAL(cl.b.dimension(0), conv_layer_type::output_dims::d);
}

BOOST_AUTO_TEST_CASE(test_fully_connected_layer_fprop) {

}

BOOST_AUTO_TEST_CASE(test_fully_connected_layer_bprop) {

}

BOOST_AUTO_TEST_CASE(test_conv_layer_forward_prop) {
    /**
     * Based on tiny-dnn unit test to verify compatibility
     * @see https://github.com/tiny-dnn/tiny-dnn/blob/master/test/test_convolutional_layer.h
     */
    using input_dims = volume_dims<1, 5, 5>;
    using k_params = kernel_symmetric_params<1, 3, 2, padding_type::valid>;
    using conv_layer_type = conv_layer<input_dims, k_params>;
    conv_layer_type layer;
    output_type real_output(1, 1, 2, 2);
    output_type expected_output(1, 1, 2, 2);
    input_type input(1, 1, 5, 5);

    input.setValues(
        {
            {
                {
                    {0.0f, 1.0f, 2.0f, 3.0f, 4.0f},
                    {1.0f, 2.0f, 3.0f, 4.0f, 5.0f},
                    {2.0f, 3.0f, 4.0f, 5.0f, 6.0f},
                    {3.0f, 4.0f, 5.0f, 6.0f, 7.0f},
                    {4.0f, 5.0f, 6.0f, 7.0f, 8.0f}
                }
            }
        }
    );
    expected_output.setValues(
        {
            {
                {
                    {9.5f, 18.5f},
                    {18.5f, 27.5f}
                }
            }
        }
    );
    layer.b.setValues({0.5f});
    layer.w.setValues(
        {
            {
                {
                    {0.5f, 0.5f, 0.5f},
                    {0.5f, 0.5f, 0.5f},
                    {0.5f, 0.5f, 0.5f}
                }
            }
        }
    );

    layer.forward_prop(input, real_output);
    assert_tensor_equals(expected_output, real_output);
}

BOOST_AUTO_TEST_CASE(test_conv_layer_backward_prop) {
    /**
     * Based on tiny-dnn unit test to verify compatibility
     * (convolutional, with_stride, bd3f62f)
     * see https://github.com/tiny-dnn/tiny-dnn/blob/master/test/test_convolutional_layer.h
     */
    using input_dims = volume_dims<1, 5, 5>;
    using k_params = kernel_symmetric_params<1, 3, 2, padding_type::valid>;
    using conv_layer_type = conv_layer<input_dims, k_params, true>;

    conv_layer_type layer;

    /* Layer weight and bias override */
    layer.b.setValues({0.5f});
    layer.w.setValues({{{
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f}
    }}});

    /* Input values for forward propagation */
    input_type prev_out(1, 1, 5, 5);
    prev_out.setValues({{{
        {0.0f, 1.0f, 2.0f, 3.0f, 4.0f},
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f},
        {2.0f, 3.0f, 4.0f, 5.0f, 6.0f},
        {3.0f, 4.0f, 5.0f, 6.0f, 7.0f},
        {4.0f, 5.0f, 6.0f, 7.0f, 8.0f}
    }}});

    /* Inputs for backward propagation and initialization */
    previous_output_type curr_out(1, 1, 5, 5);
    previous_delta_type prev_delta(1, 1, 5, 5);
    current_delta_type curr_delta(1, 1, 2, 2);

    prev_delta.setZero();
    curr_delta.setValues({{{
        {-1.0f, 2.0f },
        { 3.0f, 0.0f }
    }}});

    /* Expected and initialize zero */
    previous_delta_type expected_prev_delta(1, 1, 5, 5);
    weights_delta_type expected_dw(1, 1, 1, 3, 3);
    bias_delta_type expectd_db(1, 1);

    expected_prev_delta.setValues({{{
        {-0.5f, -0.5f, 0.5f, 1.0f, 1.0f },
        {-0.5f, -0.5f, 0.5f, 1.0f, 1.0f },
        { 1.0f,  1.0f, 2.0f, 1.0f, 1.0f },
        { 1.5f,  1.5f, 1.5f, 0.0f, 0.0f },
        { 1.5f,  1.5f, 1.5f, 0.0f, 0.0f },
    }}});

    expected_dw.setValues({{{{
        {10.0f, 14.0f, 18.0f},
        {14.0f, 18.0f, 22.0f},
        {18.0f, 22.0f, 26.0f}
    }}}});

    expectd_db.setValues({{4.0f}});

    layer.forward_prop(prev_out, curr_out);

    layer.backward_prop(prev_out, prev_delta, curr_out, curr_delta);

    assert_tensor_equals(expected_prev_delta, prev_delta);
    assert_tensor_equals(expected_dw, layer.dw);
    assert_tensor_equals(expectd_db, layer.db);
}


BOOST_AUTO_TEST_CASE(test_activation_tanh_fprop_bprop) {
    /**
     * Building upon previous unit test, test adding activation (tanh) to
     * convolution layer
     */
    using input_dims = volume_dims<1, 5, 5>;
    using k_params = kernel_symmetric_params<1, 3, 2, padding_type::valid>;
    using conv_layer_type = conv_layer<input_dims, k_params, true>;

    using tanh_act_layer = tanh_layer<volume_dims<1, 3, 3>>;
    conv_layer_type layer;
    tanh_act_layer act_layer;

    /* Layer weight and bias override */
    layer.b.setValues({0.5f});
    layer.w.setValues({{{
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f}
    }}});

    input_type prev_out(1, 1, 5, 5);
    previous_delta_type prev_delta(1, 1, 5, 5);
    previous_output_type curr_out(1, 1, 2, 2);
    current_delta_type curr_delta(1, 1, 2, 2);

    prev_out.setValues({{{
        {0.0f, 1.0f, 2.0f, 3.0f, 4.0f},
        {1.0f, 2.0f, 3.0f, 4.0f, 5.0f},
        {2.0f, 3.0f, 4.0f, 5.0f, 6.0f},
        {3.0f, 4.0f, 5.0f, 6.0f, 7.0f},
        {4.0f, 5.0f, 6.0f, 7.0f, 8.0f}
    }}});
    prev_delta.setZero();
    curr_delta.setValues({{{
        {-1.0f, 2.0f },
        { 3.0f, 0.0f }
    }}});

    /* Expected and initialize zero */
    previous_delta_type expected_prev_delta(1, 1, 5, 5);
    weights_delta_type expected_dw(1, 1, 1, 3, 3);
    bias_delta_type expectd_db(1, 1);

    expected_prev_delta.setValues({{{
        {-0.5f, -0.5f, 0.5f, 1.0f, 1.0f },
        {-0.5f, -0.5f, 0.5f, 1.0f, 1.0f },
        { 1.0f,  1.0f, 2.0f, 1.0f, 1.0f },
        { 1.5f,  1.5f, 1.5f, 0.0f, 0.0f },
        { 1.5f,  1.5f, 1.5f, 0.0f, 0.0f },
    }}});

    expected_dw.setValues({{{{
        {10.0f, 14.0f, 18.0f},
        {14.0f, 18.0f, 22.0f},
        {18.0f, 22.0f, 26.0f}
    }}}});

    expectd_db.setValues({{4.0f}});

    output_type act_out(1, 1, 3, 3);
    output_type expected_act_out(1, 1, 3, 3);

    output_type act_delta(1, 1, 3, 3);
    output_type expected_act_out_delta(1, 1, 3, 3);

    /* instantite the activation op */
    auto op = tanh_act_layer::activation_op_type();
    auto dop = tanh_act_layer::activation_op_deriv_type();

    /* get curr_out from convolution layer */
    layer.forward_prop(prev_out, curr_out);

    /* manually calculate expected activation out */
    expected_act_out.setValues({{{ //Simply performs acti(x) for every value in input
        {op(curr_out(0, 0, 0, 0)), op(curr_out(0, 0, 0, 1)), op(curr_out(0, 0, 0, 2))},
        {op(curr_out(0, 0, 1, 0)), op(curr_out(0, 0, 1, 1)), op(curr_out(0, 0, 1, 2))},
        {op(curr_out(0, 0, 2, 0)), op(curr_out(0, 0, 2, 1)), op(curr_out(0, 0, 2, 2))},
    }}});

    /* perform activation forward propagation*/
    act_layer.forward_prop(curr_out, act_out);

    BOOST_REQUIRE((std::is_same_v<tanh_act_layer::activation_op_type, tanh_activation_op>));

    assert_tensor_equals(expected_act_out, act_out);

    layer.backward_prop(prev_out, prev_delta, curr_out, curr_delta);
    act_layer.backward_prop(act_out, act_delta, prev_out, prev_delta);

    expected_act_out_delta.setValues({{{ //act_delta(x, y, ...) = prev_delta(x, y, ...) * deriv_op(prev_delta(x, y, ...))
        {prev_delta(0, 0, 0, 0) * dop(prev_out(0, 0, 0, 1)), prev_delta(0, 0, 0, 1) * dop(prev_out(0, 0, 0, 1)), prev_delta(0, 0, 0, 2) * dop(prev_out(0, 0, 0, 2))},
        {prev_delta(0, 0, 1, 0) * dop(prev_out(0, 0, 1, 0)), prev_delta(0, 0, 1, 1) * dop(prev_out(0, 0, 1, 1)), prev_delta(0, 0, 1, 2) * dop(prev_out(0, 0, 1, 2))},
        {prev_delta(0, 0, 2, 0) * dop(prev_out(0, 0, 2, 0)), prev_delta(0, 0, 2, 1) * dop(prev_out(0, 0, 2, 1)), prev_delta(0, 0, 2, 2) * dop(prev_out(0, 0, 2, 2))},
    }}});

    assert_tensor_equals(expected_act_out_delta, act_delta);
}

BOOST_AUTO_TEST_CASE(test_pooling_layer_mean_fprop_stride) {

    using input_dims = volume_dims<1, 4, 4>;
    using k_params = pooling_kernel_params<2, 2>;

    using mean_pooling_layer_type = mean_pooling_layer<input_dims, k_params>;

    mean_pooling_layer_type layer;

    /* Layer weight and bias override */
    input_type input(1, 1, 4, 4);
    input.setValues({{{
        {0,  1,  2,  3},
        {8,  7,  5,  6},
        {4,  3,  1,  2},
        {0, -1, -2, -3}
    }}});

    output_type result(1, 1, 2, 2);

    output_type expected_result(1, 1, 2, 2);

    expected_result.setValues({{{
        { 4,    4  },
        { 1.5, -0.5},
    }}});

    layer.forward_prop(input, result);

    assert_tensor_equals(expected_result, result);

}

BOOST_AUTO_TEST_CASE(test_pooling_layer_mean_bprop_stride) {

    using input_dims = volume_dims<1, 4, 4>;
    using k_params = pooling_kernel_params<2, 2>;

    using avg_pooling_layer_type = mean_pooling_layer<input_dims, k_params>;

    avg_pooling_layer_type layer;

    /* assumes weight is one */
    BOOST_REQUIRE_EQUAL((layer.w(0, 0, 0, 0)), 1);

    /* Layer weight and bias override */
    previous_output_type prev_out(1, 1, 4, 4);
    previous_output_type curr_out(1, 1, 2, 2); curr_out.setZero();
    previous_delta_type prev_delta(1, 1, 4, 4);
    previous_delta_type expected_prev_delta(1, 1, 4, 4);
    current_type curr_delta(1, 1, 2, 2);

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

    output_type result(1, 1, 2, 2);

    layer.forward_prop(prev_out, result);

    layer.backward_prop(prev_out, prev_delta, curr_out, curr_delta);

    assert_tensor_equals(expected_prev_delta, prev_delta);

}

BOOST_AUTO_TEST_CASE(test_pooling_layer_max_bprop_stride) {

    using input_dims = volume_dims<1, 4, 4>;
    using k_params = pooling_kernel_params<2, 2>;
    using max_pooling_layer_type = max_pooling_layer<input_dims, k_params>;

    max_pooling_layer_type layer;

    previous_output_type prev_out(1, 1, 4, 4);
    previous_output_type curr_out(1, 1, 2, 2); curr_out.setZero();
    previous_delta_type prev_delta(1, 1, 4, 4);
    previous_delta_type expected_prev_delta(1, 1, 4, 4);
    current_type curr_delta(1, 1, 2, 2);

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
BOOST_AUTO_TEST_CASE(test_train_fc_update) {

    sample_vector_type samples(10, sample_type(1, 1, 1));
    for(size_t s = 0; s < 10; ++s) {
        samples[s].setRandom();
    }
    class_vector_type classes{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    using network_def = network<fully_connected_layer<volume_dims<1, 1, 1>, 10>>;
    network_def network;

    training<2> train;

    train(network, samples, classes);

}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(nn_utils)
BOOST_AUTO_TEST_CASE(test_normalize_class_vector_sequential) {

    class_vector_type classes{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const size_t batch_size = 2;
    using network_def = network<fully_connected_layer<volume_dims<1, 1, 1>, 10>>;
    network_def network;

    auto [min, max] = network.out_range();

    auto result = normalize(network, batch_size, classes);

    BOOST_REQUIRE_EQUAL(result.size(), 5);
    for(size_t b = 0; b < 5; ++b) {
        /* each batch contains 20 samples */
        BOOST_REQUIRE_EQUAL(result[b].size(), 20);
        for(size_t s = 0; s < 2; ++s) {
            for(auto& cl : classes) {
                if(classes[b * 2 + s] == cl) {
                    BOOST_REQUIRE_EQUAL(result[b](s, cl, 0, 0), max);
                } else {
                    BOOST_REQUIRE_EQUAL(result[b](s, cl, 0, 0), min);
                }
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(test_normalize_class_vector_nonesequential) {

    class_vector_type classes{5, 1, 8, 3, 9, 2, 7, 4, 6, 99};
    const size_t batch_size = 2;
    using network_def = network<fully_connected_layer<volume_dims<1, 1, 1>, 150>>;
    network_def network;

    auto [min, max] = network.out_range();

    auto result = normalize(network, batch_size, classes);

    BOOST_REQUIRE_EQUAL(result.size(), 5);
    for(size_t b = 0; b < 5; ++b) {
        BOOST_REQUIRE_EQUAL(result[b].size(), 300);
        for(size_t s = 0; s < 2; ++s) {
            for(size_t o = 0; o < 150; ++o) {
                if(classes[b * 2 + s] == o) {
                    BOOST_REQUIRE_EQUAL(result[b](s, o, 0, 0), max);
                } else {
                    BOOST_REQUIRE_EQUAL(result[b](s, o, 0, 0), min);
                }
            }
        }
    }
}
BOOST_AUTO_TEST_CASE(test_normalize_sample_vector) {
    const size_t sample_count = 10;
    sample_vector_type samples(sample_count, sample_type(1, 1, 1));
    for(size_t s = 0; s < sample_count; ++s) {
        samples[s].setRandom();
    }
    class_vector_type classes{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    using network_def = network<fully_connected_layer<volume_dims<1, 1, 1>, 10>>;
    network_def network;

    auto res = normalize(network, 2, samples);
    BOOST_REQUIRE_EQUAL(res.size(), 5);
}


BOOST_AUTO_TEST_SUITE_END()