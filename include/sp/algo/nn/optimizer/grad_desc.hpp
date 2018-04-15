/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_OPTIMIZER_GRAD_DESC_HPP
#define SP_ALGO_NN_OPTIMIZER_GRAD_DESC_HPP

#include "optimizer.hpp"

#include <boost/assert.hpp>

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Implementation of gradient descent optimizer
 */

/**
 * \brief Gradient Descent Optimizer
 */
template<typename LearningRate = default_learning_rate>
struct gradient_descent_optimizer : optimizer<gradient_descent_optimizer<LearningRate>> {

    constexpr static float_t alpha = static_cast<float_t>(LearningRate::num) / static_cast<float_t>(LearningRate::den);

    static_assert(alpha > 0 && alpha < 1.0, "Alpha is in the range of (0, 1]");

    constexpr static float_t epsilon = 1.0e-8f;

    template<typename Tensor>
    void update_impl(Tensor& dw, Tensor& w) {
        BOOST_ASSERT(dw.size() == w.size());
        BOOST_ASSERT(dw.dimensions() == w.dimensions());
        w -= (alpha * dw);
    }

};

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_OPTIMIZER_GRAD_DESC_HPP */

