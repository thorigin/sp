/**
 * Copyright (C). All Rights Reserved.
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 *
 * Written by
 * - Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 */

#ifndef SP_UTIL_DATASET_FUNCTION_HPP
#define SP_UTIL_DATASET_FUNCTION_HPP

#include "mnist_data_reader.hpp"

SP_UTIL_NAMESPACE_BEGIN

/**
 * \brief Loads in four parts of the mnist data set.
 */
void load_mnist(
    const std::string& directory,
    mnist_images_result& training_images,
    mnist_labels_result& training_labels,
    mnist_images_result& testing_images,
    mnist_labels_result& testing_labels
    ) {

    std::string training_images_file = directory + "/train-images-idx3-ubyte";
    std::string training_labels_file = directory + "/train-labels-idx1-ubyte";
    std::string testing_images_file = directory + "/t10k-images-idx3-ubyte";
    std::string testing_labels_file = directory + "/t10k-labels-idx1-ubyte";

    mnist_data_reader training_reader(training_images_file, training_labels_file);

    mnist_data_reader testing_reader(testing_images_file, testing_labels_file);

    training_images = training_reader.read_images();
    training_labels = training_reader.read_labels();

    testing_images = testing_reader.read_images();
    testing_labels = testing_reader.read_labels();

}

SP_UTIL_NAMESPACE_END


#endif /* SP_UTIL_DATASET_FUNCTION_HPP */