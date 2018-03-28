/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_ACTIVATION_TANH_HPP
#define SP_ALGO_NN_LAYER_ACTIVATION_TANH_HPP

#include "layer.hpp"
#include "op.hpp"

/**
 * @file Implements tanh activation layer using alias
 *       and activation_op definition
 */

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * @brief Tanh activation layer (alias)
 */
template<typename InputVolume>
using tanh_layer = activation_layer<InputVolume, tanh_activation_op>;

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_ACTIVATION_TANH_HPP */

