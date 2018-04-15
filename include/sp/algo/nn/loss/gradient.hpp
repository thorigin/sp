/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */
#ifndef SP_ALGO_NN_LOSS_GRADIENT_HPP
#define SP_ALGO_NN_LOSS_GRADIENT_HPP

#include <boost/assert.hpp>
#include "../config.hpp"
#include "../matrix.hpp"
#include "../types.hpp"
#include "sp/util/hints.hpp"


SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \file Gradient helper funciton
 */

/**
 * \brief Calculate the the gradient of the expected vs actual output of
 *        a mini-batch
 */
template<typename LossFunction>
void gradient(LossFunction& loss, tensor_4& predicted, tensor_4& expected, tensor_4& result) {
    BOOST_ASSERT_MSG(
        predicted.dimensions() == expected.dimensions(),
        "Dimensions of Expected and Actual must match"
    );
    const size_t sample_count = predicted.dimension(0);
    for(size_t si = 0; si < sample_count; ++si) {
        loss.derivative(si, predicted, expected, result);
    }
}

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LOSS_GRADIENT_HPP */

