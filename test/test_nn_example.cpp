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
#include <boost/test/included/unit_test.hpp>

#include "sp/algo/nn.hpp"
#include "sp/testing/assert_matrix.hpp"
#include "sp/algo/nn/optimizer/ada_grad.hpp"
using namespace sp::algo::nn;
using namespace sp::testing;


BOOST_AUTO_TEST_SUITE( test_conv )

BOOST_AUTO_TEST_CASE(test_conv_nn_simple_mnist, * boost::unit_test::disabled()) {
    /**
     * MNIST NN example, based on LeCun 1998
     */

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
                6, 16, //6x16, defined as:
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
            kernel_symmetric_params<120, 1, 1>
        >,
        tanh_layer<volume_dims<120, 1, 1>>,
        fully_connected_layer<
            volume_dims<120>,
            120
        >,
        tanh_layer<volume_dims<10>>
    >;
    
    network_def nn;

    sample_vector_type samples;
    class_vector_type labels;

    training<> trainer;

    trainer(nn, samples, labels);

    std::cout << "Loading MNIST data...\n";
    
    class_vector_type  train_labels, test_labels;
    sample_vector_type train_images, test_images;   
    
}
BOOST_AUTO_TEST_SUITE_END()