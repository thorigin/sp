/**
 * Copyright (C). All Rights Reserved.
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 *
 * Written by
 * - Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 * - Jacob Escobedo <jacob@jmesco.com>, 2018
 */

#include <iostream>
#include "sp/util/dataset.hpp"
#include "sp/util/typename.hpp"
#define BOOST_TEST_MODULE sp_util_dataset
#include <boost/test/unit_test.hpp>

using namespace sp::util;

BOOST_AUTO_TEST_CASE( mnist_data_reader_construct_success) {

    mnist_data_reader mdr("resources/mnist/train-images-idx3-ubyte",  "resources/mnist/train-labels-idx1-ubyte");

    BOOST_REQUIRE_EQUAL(mdr.size(), 60000);

}

BOOST_AUTO_TEST_CASE( mnist_data_reader_read_images) {

    mnist_data_reader mdr("resources/mnist/train-images-idx3-ubyte",  "resources/mnist/train-labels-idx1-ubyte");


    auto res = mdr.read_images();
    BOOST_REQUIRE_EQUAL(mdr.frame_size(), 60000);
    BOOST_REQUIRE_EQUAL(mdr.image_height(), 28);
    BOOST_REQUIRE_EQUAL(mdr.image_width(), 28);
    BOOST_REQUIRE_EQUAL(res.size(), mdr.frame_size());
}

BOOST_AUTO_TEST_CASE( mnist_data_reader_read_labels) {

    mnist_data_reader mdr("resources/mnist/train-images-idx3-ubyte",  "resources/mnist/train-labels-idx1-ubyte");


    auto res = mdr.read_labels();
    BOOST_REQUIRE_EQUAL(mdr.frame_size(), 60000);
    BOOST_REQUIRE_EQUAL(mdr.image_height(), 28);
    BOOST_REQUIRE_EQUAL(mdr.image_width(), 28);
    BOOST_REQUIRE_EQUAL(res.size(), mdr.frame_size());
}

BOOST_AUTO_TEST_CASE( mnist_load_mnist_dataset) {

    mnist_images_result training_images, testing_images;
    mnist_labels_result training_labels, testing_labels;

    load_mnist("resources/mnist", training_images, training_labels, testing_images, testing_labels);

    BOOST_REQUIRE(!training_images.empty());
    BOOST_REQUIRE(!testing_images.empty());
    BOOST_REQUIRE(!training_labels.empty());
    BOOST_REQUIRE(!testing_labels.empty());
    BOOST_REQUIRE_EQUAL(training_images.size(), training_labels.size());
    BOOST_REQUIRE_EQUAL(testing_images.size(), testing_labels.size());
}

BOOST_AUTO_TEST_CASE( mnist_load_mnist_dataset_labels_complete,
                    * boost::unit_test::label("regression")) {

    mnist_images_result training_images, testing_images;
    mnist_labels_result training_labels, testing_labels;

    load_mnist("resources/mnist", training_images, training_labels, testing_images, testing_labels);

    BOOST_REQUIRE_EQUAL(training_labels[0], 5);
    BOOST_REQUIRE_EQUAL(training_labels[1], 0);
    BOOST_REQUIRE_EQUAL(training_labels[2], 4);
    BOOST_REQUIRE_EQUAL(training_labels[3], 1);
    BOOST_REQUIRE_EQUAL(training_labels[4], 9);
    //...
    BOOST_REQUIRE_EQUAL(training_labels[59995], 8);
    BOOST_REQUIRE_EQUAL(training_labels[59996], 3);
    BOOST_REQUIRE_EQUAL(training_labels[59997], 5);
    BOOST_REQUIRE_EQUAL(training_labels[59998], 6);
    BOOST_REQUIRE_EQUAL(training_labels[59999], 8);
}


BOOST_AUTO_TEST_CASE( mnist_load_mnist_dataset_images_unpadded_dimensions) {

    mnist_images_result training_images, testing_images;
    mnist_labels_result training_labels, testing_labels;

    load_mnist("resources/mnist", training_images, training_labels, testing_images, testing_labels);

    for(auto& img : training_images) {
        BOOST_REQUIRE_EQUAL(img.dimensions()[0], 1);
        BOOST_REQUIRE_EQUAL(img.dimensions()[1], 28);
        BOOST_REQUIRE_EQUAL(img.dimensions()[2], 28);
    }

    for(auto& img : testing_images) {
        BOOST_REQUIRE_EQUAL(img.dimensions()[0], 1);
        BOOST_REQUIRE_EQUAL(img.dimensions()[1], 28);
        BOOST_REQUIRE_EQUAL(img.dimensions()[2], 28);
    }
}

BOOST_AUTO_TEST_CASE( mnist_load_mnist_dataset_images_padded_dimensions) {

    mnist_images_result training_images, testing_images;
    mnist_labels_result training_labels, testing_labels;

    load_mnist("resources/mnist", training_images, training_labels, testing_images, testing_labels, {-1.0f, 1.0f}, 2, 2);

    for(auto& img : training_images) {
        BOOST_REQUIRE_EQUAL(img.dimensions()[0], 1);
        BOOST_REQUIRE_EQUAL(img.dimensions()[1], 32);
        BOOST_REQUIRE_EQUAL(img.dimensions()[2], 32);
    }

    for(auto& img : testing_images) {
        BOOST_REQUIRE_EQUAL(img.dimensions()[0], 1);
        BOOST_REQUIRE_EQUAL(img.dimensions()[1], 32);
        BOOST_REQUIRE_EQUAL(img.dimensions()[2], 32);
    }
}