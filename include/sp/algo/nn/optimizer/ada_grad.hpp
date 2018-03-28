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

SP_ALGO_NN_NAMESPACE_BEGIN

namespace detail {
    constexpr float_t epsilon = 1e-9;
}

/**
 * \brief Default learning rate
 */
using default_learning_rate = std::ratio<1, 100>;

/**
 * \brief Adaptive Gradient Optimizer
 *
 * \tparam LearningRatio, a std::ratio instantiation designating the learning ratio
 */
template<typename LearningRate = default_learning_rate>
struct ada_gradient_optimizer : stateful_optimizer<1, ada_gradient_optimizer<LearningRate>> {

    constexpr static float_t alpha = static_cast<float_t>(LearningRate::num) / static_cast<float_t>(LearningRate::den);

    void update_impl(weights_type& dw, weights_type& w) {

        auto& g = this->state[0][&dw];

        if(g.dimensions() != w.dimensions()) {
            g.resize(w.dimensions());
            g.setZero();
        }

        g += dw * dw;
        weights_type epsilon(1, 1, 1, 1);
        epsilon.setConstant(detail::epsilon);

        w -= alpha * dw / (g + epsilon.broadcast(g.dimensions())).sqrt();
    }


};

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_OPTIMIZER_ADA_GRAD_HPP */

