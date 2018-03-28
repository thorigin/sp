/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_LAYER_HPP
#define SP_ALGO_NN_LAYER_LAYER_HPP

#include "../config.hpp"
#include "../matrix.hpp"
#include "../types.hpp"
#include "params.hpp"
#include "sp/util/types.hpp"
#include "activation/op.hpp"
#include "detail/layers.hpp"

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \file Basic layer abstract class
 */

/**
 * \brief Abstract layer of neural network
 * 
 * All possible layer types extend this base type.
 *
 * \tparam Attached, if layer shares state with previous layer
 * \tparam DefaultOutputRange when false, must implement out_range
 */
template<
    typename InputVolume,
    typename OutputVolume,
    typename Derived,
    bool DefaultOutputRange = true
>
struct layer {

    using derived_type = Derived;

    /**
     * \brief Input Dimensions
     */
    using input_dims = InputVolume;

    /**
     * \brief Output Dimensions
     */
    using output_dims = OutputVolume;

    constexpr static bool default_output_range = DefaultOutputRange;

    /**
     * Validate InputDims
     */
    static_assert(util::is_instantiation_of_v<InputVolume, volume_dims>, "InputDim template parameter must be an instance of input_dims");

    /**
     * Validate OutputDims
     */
    static_assert(util::is_instantiation_of_v<OutputVolume, volume_dims>, "InputDim template parameter must be an instance of input_dims");

    layer() : is_trainable(true) {}
    /** Deleted */
    layer(const layer&) = delete;
    /** Deleted */
    layer(layer&&) = delete;
    /** Deleted */
    layer& operator=(const layer&) = delete;
    /** Deleted */
    layer& operator=(layer&&) = delete;

    /**
     * Feed forward propagation
     *
     * \param input The input tensor
     */
    void forward_prop(input_type& input, output_type& output) {
        derived().forward_prop_impl(input, output);
    }

    /**
     * Backward propagation
     * \param input The input tensor
     * \param output The output tensor
     */
    void backward_prop(     previous_output_type& prev_out,
                        previous_delta_type& prev_delta,
                        current_type& curr_out,
                        current_delta_type& curr_delta) {
        derived().backward_prop_impl(prev_out, prev_delta, curr_out, curr_delta);
    }

    template<typename Optimizer>
    void update_weights(Optimizer& optimizer) {
        if constexpr (detail::has_delta_weight_v<derived_type>) {
            auto& dw = derived().dw;
            const size_t samples = dw.dimension(0);

            deltas.resize(
                dw.dimension(1),
                dw.dimension(2),
                dw.dimension(3),
                dw.dimension(4)
            );

            deltas = dw.chip(0, 0);
            for(size_t s = 1; s < samples; ++s) {
                deltas += dw.chip(s, 0);
            }

            tensor_4 reciprocal_batch_size(1, 1, 1, 1);
            reciprocal_batch_size.setConstant(1.0f / samples);

            deltas *= reciprocal_batch_size.broadcast(deltas.dimensions());
            weights_type& w = derived().w;
            optimizer.update(deltas, w);
        }

    }

    void clear_gradients() {
        if constexpr (detail::has_delta_weight_v<derived_type>) {
            derived().dw.setZero();
        }
        if constexpr (detail::has_delta_bias_v<derived_type>) {
            derived().db.setZero();
        }
    }

    /**
     * CRTP helper
     */
    const derived_type& derived() const {
        return *static_cast<const derived_type*>(this);
    }

    /**
     * CRTP helper
     */
    derived_type& derived() {
        return *static_cast<derived_type*>(this);
    }

    /**
     * \brief Returns the output range of a layer     
     */
    valid_range
    range() {
        if constexpr(default_output_range) {
            return {-1.0f, 1.0f};
        } else {
            return derived().range_impl();
        }
    }

    /**
     * Whether or not a layer is trainable
     */
    bool is_trainable;

    /**
     * \brief Temporarily used in combine_grads
     */
    tensor_4 deltas;
};

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_LAYER_HPP */

