/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_TRAINING_HPP
#define SP_ALGO_NN_TRAINING_HPP

#include <tuple>
#include <array>
#include <fstream>
#include <iosfwd>
#include <experimental/filesystem>

#include "sp/util/timing.hpp"

#include "sp/util/for_each.hpp"
#include "sp/util/tuples.hpp"
#include "sp/util/hints.hpp"
#include "sp/util/typename.hpp"

#include "sp/config.hpp"
#include "matrix.hpp"
#include "loss.hpp"
#include "optimizer.hpp"
#include "types.hpp"
#include "normalize.hpp"
#include "weight.hpp"


SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \file Definitions of network composite structure and common typedefs
 */

/**
 * \brief Training
 */
template<
    size_t BatchSize = 16,
    size_t Epochs = 1,
    typename Optimizer = ada_gradient_optimizer<>,
    typename LossFunction = mean_square_error,
    typename OnEpoch = void
>
struct training {

    /**
     * Optimizer type
     */
    using optimizer_type = Optimizer;

    /**
     * Loss function type
     */
    using loss_function_type = LossFunction;

    /**
     * Batch size
     */
    constexpr static size_t batch_size = BatchSize;

    static_assert(BatchSize > 0, "BatchSize is greater or equal to 1");

    /**
     * Number of training epochs
     */
    constexpr static size_t epochs = Epochs;

    template<typename Network>
    sp_hot void operator()(Network& network, sample_vector_type& samples, class_vector_type& classes) {

        BOOST_ASSERT_MSG(!samples.empty(), "Samples is not empty");
        BOOST_ASSERT_MSG(samples.size() == classes.size(), "Samples and classes size match");

        const size_t batch_count = samples.size() / batch_size;

        /**
         * \TODO implement remaining batch handling
         */
        BOOST_ASSERT_MSG(samples.size()  % batch_size == 0, "input size is divisble by batch size");

        std::vector<tensor_4> expected_outputs = prepare_labels(network, batch_size, classes);

        network.configure(batch_size, true);

        cached_gradient.resize(
            batch_size,
            Network::output_dims::d,
            Network::output_dims::h,
            Network::output_dims::w
        );
        cached_input_batch.resize(
            batch_size,
            Network::input_dims::d,
            Network::input_dims::h,
            Network::input_dims::w
        );

        for(size_t i = 0; i < epochs; ++i) {
            for(size_t b = 0; b < batch_count; ++b) {

                /* prepare samples into a batch, copying */
                for(size_t s = 0; s < batch_size; ++s) {
                    auto& sample = samples[b * batch_size + s];
                    BOOST_ASSERT(detail::validate_dimensions<typename Network::input_dims>(sample));
                    cached_input_batch.chip(s, 0) = sample;
                }

                (*this)(network, cached_input_batch, expected_outputs[b]);

                if(on_batch) {
                    on_batch(b);
                }
            }
            if(on_epoch) {
                on_epoch(i);
            }
        }
    }

    /**
     * \brief Perform a single training operation
     */
    template<typename Network>
    sp_hot void operator()(Network& network, tensor_4& input, tensor_4& expected_output) {
        tensor_4& predicted = network.forward(input);
        gradient(loss, predicted, expected_output, cached_gradient);
        network.backward(cached_gradient);
        network.template update_weights(optimizer);
    }

    optimizer_type optimizer;
    loss_function_type loss;

    /**
     * \brief Can be bound to a function which is then executed
     *        on after each epoch
     */
    std::function<void(const size_t&)> on_epoch;

    /**
     * \brief Can be bound to a function which is then executed
     *        on before each batch
     */
    std::function<void(const size_t&)> on_batch;

protected:

    /* Cache gradient tensor between iterations, reduce allocations */
    tensor_4 cached_gradient;

    /* cache input batch between iterations, reduce allocations */
    tensor_4 cached_input_batch;
};

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_TRAINING_HPP */

