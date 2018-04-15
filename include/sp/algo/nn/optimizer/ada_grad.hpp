/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_OPTIMIZER_ADA_GRAD_HPP
#define SP_ALGO_NN_OPTIMIZER_ADA_GRAD_HPP

#include "../config.hpp"
#include "optimizer.hpp"

#include <boost/assert.hpp>

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Adaptive Gradient Optimizer
 *
 * \tparam LearningRatio, a std::ratio instantiation designating the learning ratio
 */
template<typename LearningRate = default_learning_rate>
struct ada_gradient_optimizer : stateful_optimizer<1, ada_gradient_optimizer<LearningRate>> {

    constexpr static float_t alpha = static_cast<float_t>(LearningRate::num) / static_cast<float_t>(LearningRate::den);
    static_assert(alpha > 0 && alpha < 1.0, "Alpha is in the range of (0, 1;");

    constexpr static float_t epsilon = 1.0e-8f;

    template<typename Tensor>
    void update_impl(Tensor& dw, Tensor& w) {
        tensor_1& g = this->state[0][&dw];

        auto& dims = w.dimensions();

        /* initialize g size */
        if(g.size() != w.size()) {
            g.resize(w.size());
            g.setZero();
        }

        BOOST_ASSERT(g.size() == w.size());
        BOOST_ASSERT(g.size() == dw.size());
        BOOST_ASSERT(dw.size() == w.size());
        BOOST_ASSERT(dw.dimensions() == w.dimensions());

        g.reshape(dims) += dw.square();
        w -= (alpha * dw) / (g.reshape(dims).sqrt() + epsilon);
    }


};

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_OPTIMIZER_ADA_GRAD_HPP */

