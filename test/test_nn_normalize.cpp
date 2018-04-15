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
#include "assert_matrix.hpp"

using namespace sp::algo::nn;
using namespace sp::testing;

BOOST_AUTO_TEST_CASE(test_normalize_class_vector_sequential) {

    class_vector_type classes{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const size_t batch_size = 2;
    using network_def = network<fully_connected_layer<volume_dims<1, 1, 1>, 10>>;
    network_def network;

    auto [min, max] = network.out_target_range();

    auto result = prepare_labels(network, batch_size, classes);

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

    auto [min, max] = network.out_target_range();

    auto result = prepare_labels(network, batch_size, classes);

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

    auto res = prepare_batch(network, 2, samples);
    BOOST_REQUIRE_EQUAL(res.size(), 5);
}
