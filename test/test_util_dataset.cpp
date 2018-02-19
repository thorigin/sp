/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#include <iostream>
#include "sp/util/dataset.hpp"

//#include "sp/algo/stats.hpp"
//#include "sp/algo/gen.hpp"
//#include "sp/util/units.hpp"

//#include "sp/util/timing.hpp"
//#include "sp/util/query.hpp"
#include "sp/util/typename.hpp"
#define BOOST_TEST_MODULE sp_util_dataset
#include <boost/test/unit_test.hpp>

using namespace sp::util;

BOOST_AUTO_TEST_CASE( mnist_data_reader_construct_success) {

    mnist_data_reader mdr("resources/mnist/train-images-idx3-ubyte",  "resources/mnist/train-labels-idx1-ubyte");

     BOOST_REQUIRE_EQUAL(mdr.size(), 60000);
     
}

BOOST_AUTO_TEST_CASE( mnist_data_reader_read_frame) {

    mnist_data_reader mdr("resources/mnist/train-images-idx3-ubyte",  "resources/mnist/train-labels-idx1-ubyte");
    

    BOOST_REQUIRE_EQUAL(mdr.read_frame(), std::vector<uint8_t>{1, 2, 3, 4});
}
