/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_ACTIVATION_SOFTMAX_HPP
#define SP_ALGO_NN_LAYER_ACTIVATION_SOFTMAX_HPP

#include "layer.hpp"
#include "op.hpp"

/**
 * @file Implements tanh activation layer using alias
 *       and activation_op definition
 */

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * @brief SoftMax activation layer (alias)
 */
template<typename InputVolume>
using softmax_layer = activation_layer<InputVolume, softmax_activation_op>;

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_ACTIVATION_SOFTMAX_HPP */

