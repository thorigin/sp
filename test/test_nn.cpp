/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#include <iostream>
#include <iomanip>
#define BOOST_TEST_MODULE sp_algo_nn
#include <boost/test/unit_test.hpp>
#include "sp/algo/nn.hpp"
#include "assert_matrix.hpp"
#include "sp/algo/nn/random.hpp"

using namespace sp::algo::nn;
using namespace sp::testing;

BOOST_AUTO_TEST_CASE(test_train_fc_update) {

    sample_vector_type samples = nested_vector_to_sample_vector({
        {5.2,3.5,1.4,0.2},
        {4.9,3,1.4,0.2},
        {4.7,3.2,1.3,0.2},
        {4.6,3.1,1.5,0.2},
        {5,3.6,1.4,0.3},
        {5.4,3.9,1.7,0.4},
        {4.6,3.4,1.4,0.3},
        {5,3.4,1.5,0.2},
        {4.4,2.9,1.4,0.2},
        {4.9,3.1,1.5,0.1},
        {5.4,3.7,1.5,0.2},
        {4.8,3.4,1.6,0.2},
        {4.8,3,1.4,0.1},
        {4.3,3,1.1,0.1},
        {5.8,4,1.2,0.2},
        {5.7,4.4,1.5,0.4},
        {5.4,3.9,1.3,0.4},
        {5.1,3.5,1.4,0.3},
        {5.7,3.8,1.7,0.3},
        {5.1,3.8,1.5,0.3},
        {5.4,3.4,1.7,0.2},
        {5.1,3.7,1.5,0.4},
        {4.6,3.6,1,0.2},
        {5.1,3.3,1.7,0.5},
        {4.8,3.4,1.9,0.2},
        {5,3,1.6,0.2},
        {5,3.4,1.6,0.4},
        {5.2,3.5,1.5,0.2},
        {5.2,3.4,1.4,0.2},
        {4.7,3.2,1.6,0.2},
        {4.8,3.1,1.6,0.2},
        {5.4,3.4,1.5,0.4},
        {5.2,4.1,1.5,0.1},
        {5.5,4.2,1.4,0.2},
        {4.9,3.1,1.5,0.2},
        {5,3.2,1.2,0.2},
        {5.5,3.5,1.3,0.2},
        {4.9,3.6,1.4,0.1},
        {4.4,3,1.3,0.2},
        {5.1,3.4,1.5,0.2},
        {5,3.5,1.3,0.3},
        {4.5,2.3,1.3,0.3},
        {4.4,3.2,1.3,0.2},
        {5,3.5,1.6,0.6},
        {5.1,3.8,1.9,0.4},
        {4.8,3,1.4,0.3},
        {5.1,3.8,1.6,0.2},
        {4.6,3.2,1.4,0.2},
        {5.3,3.7,1.5,0.2},
        {5,3.3,1.4,0.2},
        {7,3.2,4.7,1.4},
        {6.4,3.2,4.5,1.5},
        {6.9,3.1,4.9,1.5},
        {5.5,2.3,4,1.3},
        {6.5,2.8,4.6,1.5},
        {5.7,2.8,4.5,1.3},
        {6.3,3.3,4.7,1.6},
        {4.9,2.4,3.3,1},
        {6.6,2.9,4.6,1.3},
        {5.2,2.7,3.9,1.4},
        {5,2,3.5,1},
        {5.9,3,4.2,1.5},
        {6,2.2,4,1},
        {6.1,2.9,4.7,1.4},
        {5.6,2.9,3.6,1.3},
        {6.7,3.1,4.4,1.4},
        {5.6,3,4.5,1.5},
        {5.8,2.7,4.1,1},
        {6.2,2.2,4.5,1.5},
        {5.6,2.5,3.9,1.1},
        {5.9,3.2,4.8,1.8},
        {6.1,2.8,4,1.3},
        {6.3,2.5,4.9,1.5},
        {6.1,2.8,4.7,1.2},
        {6.4,2.9,4.3,1.3},
        {6.6,3,4.4,1.4},
        {6.8,2.8,4.8,1.4},
        {6.7,3,5,1.7},
        {6,2.9,4.5,1.5},
        {5.7,2.6,3.5,1},
        {5.5,2.4,3.8,1.1},
        {5.5,2.4,3.7,1},
        {5.8,2.7,3.9,1.2},
        {6,2.7,5.1,1.6},
        {5.4,3,4.5,1.5},
        {6,3.4,4.5,1.6},
        {6.7,3.1,4.7,1.5},
        {6.3,2.3,4.4,1.3},
        {5.6,3,4.1,1.3},
        {5.5,2.5,4,1.3},
        {5.5,2.6,4.4,1.2},
        {6.1,3,4.6,1.4},
        {5.8,2.6,4,1.2},
        {5,2.3,3.3,1},
        {5.6,2.7,4.2,1.3},
        {5.7,3,4.2,1.2},
        {5.7,2.9,4.2,1.3},
        {6.2,2.9,4.3,1.3},
        {5.1,2.5,3,1.1},
        {5.7,2.8,4.1,1.3}
    });
    // 0 not iris setosa, 1 is iris setosa
    class_vector_type classes{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1
    };

    constexpr size_t batch_size = 1;

    using network_def = network<
        fully_connected_layer<volume_dims<1, 1, 4>, 2>,
        tanh_layer<volume_dims<2>>
    >;

    network_def network;

    network.weight_initializer = gauss_weight_initializer(-0.05, 0.05);
    network.bias_initializer = gauss_weight_initializer(-0.05, 0.05);

    normalize_and_scale(network, samples);

    training<batch_size, 5, ada_gradient_optimizer<std::ratio<5, 100>>> trainer;

    trainer(network, samples, classes);

    auto[correct, total, cm] = network.test(samples, classes);
    (void)cm;
    BOOST_REQUIRE_EQUAL(correct, total);
}
BOOST_AUTO_TEST_CASE(test_network_persistence) {
    using network_def = network<
        conv_layer<
            volume_dims<1, 32, 32>,
            kernel_symmetric_params<6, 5, 1, padding_type::valid>
        >,
        tanh_layer<volume_dims<6, 28, 28>>,
        mean_pooling_layer<volume_dims<6, 28, 28>, pooling_kernel_params<2, 2>>,
        tanh_layer<volume_dims<6, 14, 14>>,
        conv_layer<
            volume_dims<6, 14, 14>,
            kernel_symmetric_params<16, 5, 1, padding_type::valid>,
            true,
            table_connectivity<
                6, 16, //6x16 (6 inputs to 16 outputs) defined as:
                1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1,
                1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1,
                1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1,
                0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1,
                0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1,
                0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1
            >
        >,
        tanh_layer<volume_dims<16, 10, 10>>,
        mean_pooling_layer<
            volume_dims<16, 10, 10>,
            pooling_kernel_params<2, 2>
        >,
        tanh_layer<volume_dims<16, 5, 5>>,
        conv_layer<
            volume_dims<16, 5, 5>,
            kernel_symmetric_params<120, 5, 5>
        >,
        tanh_layer<volume_dims<120, 1, 1>>,
        fully_connected_layer<
            volume_dims<120>,
            10
        >,
        tanh_layer<volume_dims<10>>
    >;

    network_def nn;

    nn.configure(1, true);

    std::stringstream buffer1;
    std::stringstream buffer2;

    nn.save(buffer1);

    buffer1.seekg(0, std::ios::beg);

    network_def nn2;
    nn2.load(buffer1);

    nn2.save(buffer2);

    buffer2.seekg(0);
    buffer1.seekg(0);

    float_t tmp1, tmp2;
    while(buffer1 >> tmp1 && buffer1 >> tmp2) {
        std::cout << "Comparing: \t"  << tmp1 << " to " << tmp2 << "\n";
        BOOST_REQUIRE_CLOSE(tmp1, tmp2, 0.01);
    }

}
