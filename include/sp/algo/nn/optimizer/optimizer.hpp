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
#include "../layer/layer.hpp"

SP_ALGO_NN_NAMESPACE_BEGIN

template<typename Derived>
struct optimizer {
    
    using derived_type = Derived;

    void update(weights_type& dw, weights_type& w) {
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

    using weight_mapped_type = std::unordered_map<weights_type*, weights_type>;

    std::array<weight_mapped_type, state_size> state;
};


SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_OPTIMIZER_HPP */

