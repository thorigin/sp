/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_TYPES_HPP
#define SP_ALGO_NN_LAYER_TYPES_HPP

#include <vector>
#include "config.hpp"
#include "matrix.hpp"


SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \file Contains common type definitions of inputs, outputs, deltas, samples,
 *       valid ranges, etc
 */

/**
 * \brief The output range of a layer
 */
using valid_range = std::pair<float_t, float_t>;

/**
 * \brief Network training sample type
 */
using sample_type = tensor_3;

/**
 * \brief A vector of samples
 */
using sample_vector_type = std::vector<sample_type>;

/**
 * \brief Class vector type
 *
 * Stores the class (index-wise) of each sample in a sample_vector_type
 */
using class_vector_type = std::vector<size_t>;

/**
 * \brief The weights type
 *
 * Size is (D_in, D_out, H_kernel, W_kernel)
 */
using weights_type = tensor_4;

/**
 * \brief The weights delta type
 *
 * Size is (Sample, D_in, D_out, H_kernel, W_kernel)
 */
using weights_delta_type = tensor_5;

/**
 * \brief The bias type
 */
using bias_type = tensor_1;

/**
 * \brief The bias delta type
 */
using bias_delta_type = tensor_2;


SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_TYPES_HPP */

