/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_NORMALIZE_HPP
#define SP_ALGO_NN_NORMALIZE_HPP

#include "sp/config.hpp"
#include "matrix.hpp"
#include "types.hpp"

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \file Utility to normalize values for the network specified.
 */


/**
 * \brief Normalize input_type given the network parameters into an input_type
 *
 * @param network
 * @return
 */
template<typename Network>
auto normalize(Network& network, const size_t& batch_size, sample_vector_type& samples) {

    const size_t input_count = samples.size();
    const size_t batch_count = input_count / batch_size;

    const auto sample_dims = samples[0].dimensions();

    std::vector<input_type> vec(
        batch_count,
        input_type(
            batch_size,
            sample_dims[0],
            sample_dims[1],
            sample_dims[2]
        )
    );
    
    /**
     * Merge input sample_type into input_type batch
     */
    for(size_t b = 0; b < batch_count; ++b) {
        for(size_t s = 0; s < batch_size; ++s) {
            BOOST_ASSERT_MSG(
                samples[b*batch_size+s].dimensions() == sample_dims,
                "Dimensions of samples are all the same"
            );
            vec[b].chip(s, 0) = samples[b*batch_size + s];
        }        
    }
    return vec;
}

namespace detail {
    /**
     * \brief Convert class vector type to a a vector of output_type
     */
    template<typename Network>
    auto normalize_label_to_vector_helper(Network& network, const size_t& batch_size, class_vector_type& classes) {

        const size_t class_count = classes.size();
        const size_t batch_count = class_count / batch_size;
        
        /* Output is a vector of output_type */
        std::vector<output_type> vec(
            /* of dimensions (1, 1, 1, the size of the output of the last layer) */
            batch_count, output_type(batch_size, Network::output_dims::size, 1, 1)
        );
       
        auto[min, max] = network.out_range();

        for(size_t b = 0; b < batch_count; ++b) {
            /* default to minimum value */
            vec[b].setConstant(min);
            for(size_t s = 0; s < batch_size; ++s) {
                const size_t idx = b * batch_size + s;
                BOOST_ASSERT_MSG(
                    classes[idx] < Network::output_dims::size,
                    "Class value can not exceeds network output size"
                );                
                /* set the max value to output layer max */
                vec[b](s, classes[idx], 0, 0) = max;
            }
        }

        return vec;
    }
}
/**
 * \brief Normalize a vector of classes
 */
template<typename Network>
std::vector<output_type> normalize(     Network& network,
                                        const size_t& batch_size,
                                        class_vector_type& classes) {
    return detail::normalize_label_to_vector_helper(network, batch_size, classes);
}

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_NORMALIZE_HPP */

