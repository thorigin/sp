/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_NORMALIZE_HPP
#define SP_ALGO_NN_NORMALIZE_HPP

#include <boost/assert.hpp>
#include "sp/config.hpp"
#include "matrix.hpp"
#include "types.hpp"
#include "layer/detail/layers.hpp"


SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \file Utility to normalize values for the network specified.
 */

/**
 * \brief Normalize and scale all values and translate to output maximum
 *
 * Note: considers all element at once for min/max of samples
 */
template<typename Network>
void normalize_and_scale(Network& network, sample_vector_type& vec) {
    auto [a, b] = network.out_range();
    float_t min = b;
    float_t max = a;
    for(const auto& v : vec) {
        //find min, max
        std::for_each(v.data(), v.data() + v.size(), [&](auto& x) {
            max = std::max(x, max);
            min = std::min(x, min);
        });
    }

    /**
     * Perform translation and scaling at the same time: First, scale value v_i
     * to [min, max], then rescale to [a, b].
     */
    for(auto& v : vec) {
        v = ((b - a) * (v - min) / ( max - min)) + a;
    }
}

/**
 * \brief Normalize input_type given the network parameters into an input_type
 *
 * @param network
 * @return
 */
template<typename Network>
auto prepare_batch(Network& network, const size_t& batch_size, sample_vector_type& samples) {
    BOOST_ASSERT(batch_size >= 1);
    BOOST_ASSERT(!samples.empty());

    const size_t input_count = samples.size();
    const size_t batch_count = input_count / batch_size;

    const auto& sample_dims = samples[0].dimensions();

    std::vector<tensor_4> vec(
        batch_count,
        tensor_4(
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
                detail::validate_dimensions<typename Network::input_dims>(samples[b*batch_size+s]),
                "Dimensions of sample matches network input dimension"
            );
            vec[b].chip(s, 0) = samples[b*batch_size + s];
        }
    }

    return vec;
}

/**
 * \brief Prepare samples into batch format of input_type given the network parameters into an input_type
 *
 * @param network
 * @return
 */
template<typename Network>
auto prepare_batch(Network& network, sample_vector_type& samples) {
    return prepare_labels(network, 1, samples);
}

/**
 * \brief Prepare a batch of input_type given the network parameters into an input_type
 *
 * @param network
 * @return
 */
template<typename Network>
auto prepare_batch(Network& network, sample_type& sample) {
    sample_vector_type vec{sample};
    return prepare_labels(network, 1, vec)[0];
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
        std::vector<tensor_4> vec(
            /* of dimensions (1, 1, 1, the size of the output of the last layer) */
            batch_count, tensor_4(batch_size, Network::output_dims::size, 1, 1)
        );

        auto[min, max] = network.out_target_range();

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
std::vector<tensor_4> prepare_labels(     Network& network,
                                        const size_t& batch_size,
                                        class_vector_type& classes) {
    return detail::normalize_label_to_vector_helper(network, batch_size, classes);
}

/**
 * \brief Normalize a vector of classes
 */
template<typename Network>
std::vector<tensor_4> prepare_batch(     Network& network,
                                        class_vector_type& classes) {
    return prepare_labels(network, 1, classes);
}



/**
 * \brief Converters a vector of a vector to a vector of sample_type
 */
sample_vector_type nested_vector_to_sample_vector(std::vector<std::vector<float_t>> vectors) {
    if(vectors.empty()) {
        return {};
    } else {
        const size_t vector_element_count = vectors[0].size();
        sample_vector_type res(vectors.size(), sample_type(1, 1, vector_element_count));
        size_t idx = 0;
        for(auto& v : vectors) {
            auto& r = res[idx];
            float_t* start = r.data();
            BOOST_ASSERT((static_cast<size_t>(std::distance(start, r.data() + r.size())) == vector_element_count));
            BOOST_ASSERT(vector_element_count == v.size());
            std::copy(v.begin(), v.end(), start);
            ++idx;
        }
        return res;
    }
}
SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_NORMALIZE_HPP */

