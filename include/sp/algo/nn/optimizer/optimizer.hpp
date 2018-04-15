/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_OPTIMIZER_HPP
#define SP_ALGO_NN_OPTIMIZER_HPP

#include <unordered_map>
#include <array>
#include <ratio>
#include "../layer/layer.hpp"

SP_ALGO_NN_NAMESPACE_BEGIN


/**
 * \brief Default learning rate (4%)
 */
using default_learning_rate = std::ratio<4, 100>;

/**
 * \brief Optimizer abstract struct
 */
template<typename Derived>
struct optimizer {

    using derived_type = Derived;

    template<typename Tensor>
    void update(Tensor& dw, Tensor& w) {
        BOOST_ASSERT_MSG(
            dw.dimensions() == w.dimensions(),
            "dw and w dimensions must match"
        );
        derived().update_impl(dw, w);
    }

    derived_type& derived() {
        return static_cast<derived_type&>(*this);
    }
};

template<size_t StateSize, typename Derived>
struct stateful_optimizer : optimizer<Derived> {

    constexpr static size_t state_size = StateSize;

    static_assert(state_size > 0, "state_size is greater than zero");

    /**
     * \brief Maps a memory address to a tensor of rank 1
     */
    using state_map_type = std::unordered_map<void*, tensor_1>;

    std::array<state_map_type, state_size> state;
};


SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_OPTIMIZER_HPP */

