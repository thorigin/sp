/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_RANDOM_HPP
#define SP_ALGO_NN_RANDOM_HPP

#include <random>
#include "config.hpp"

/**
 * \brief Provides basic abstraction and generalization of a random source
 *        for the nn namespace
 */
SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Default random generator
 */
struct random_generator {

    using random_generator_type = SP_ALGO_NN_RANDOM_GENERATOR;

    static random_generator_type& get() {
        static random_generator_type instance = random_generator_type(std::random_device{}());
        return instance;
    }

};

/**
 * \brief Get a random index in the tensor
 */
template<typename Tensor>
size_t random_index(const Tensor& tensor) {
    return random_generator::get() % tensor.size();
}

SP_ALGO_NN_NAMESPACE_END


#endif  /* SP_ALGO_NN_RANDOM_HPP */