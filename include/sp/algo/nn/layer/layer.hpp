/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_LAYER_HPP
#define SP_ALGO_NN_LAYER_LAYER_HPP

#include <iosfwd>
#include <boost/assert.hpp>
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
 * \tparam ConfiguresItself, when true, must implement
 *         '''configuration_impl(const size_t& batch_size, bool reset)'''
 *         and it must initialize weights and bias, if any
 */
template<
    typename InputVolume,
    typename OutputVolume,
    typename Derived,
    bool DefaultOutputRange = true,
    bool ConfiguresItself = false
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

    constexpr static bool configures_itself = ConfiguresItself;

    /**
     * Validate InputDims
     */
    static_assert(util::is_instantiation_of_v<InputVolume, volume_dims>, "InputDim template parameter must be an instance of input_dims");

    /**
     * Validate OutputDims
     */
    static_assert(util::is_instantiation_of_v<OutputVolume, volume_dims>, "InputDim template parameter must be an instance of input_dims");

    /**
     * Feed forward propagation
     *
     * \param input The input tensor
     */
    void forward_prop(tensor_4& input, tensor_4& output) {
        BOOST_ASSERT_MSG(
            detail::validate_dimensions<input_dims>(input),
            "Dimensions of input matches input dimension"
        );
        BOOST_ASSERT_MSG(
            detail::validate_dimensions<output_dims>(output),
            "Dimensions of output matches output dimensions"
        );
        derived().forward_prop_impl(input, output);
    }

    /**
     * Backward propagation
     * \param input The input tensor
     * \param output The output tensor
     */
    void backward_prop(     tensor_4& prev_out,
                            tensor_4& prev_delta,
                            tensor_4& curr_out,
                            tensor_4& curr_delta) {
        BOOST_ASSERT_MSG(
            detail::validate_dimensions<input_dims>(prev_out),
            "Dimensions prev_out matches input dimension"
        );
        BOOST_ASSERT_MSG(
            detail::validate_dimensions<input_dims>(prev_delta),
            "Dimensions prev_delta matches input dimension"
        );
        BOOST_ASSERT_MSG(
            detail::validate_dimensions<output_dims>(curr_out),
            "Dimensions of curr_out matches input dimensions"
        );
        BOOST_ASSERT_MSG(
            detail::validate_dimensions<output_dims>(curr_delta),
            "Dimensions of current_delta matches output dimensions"
        );
        derived().backward_prop_impl(prev_out, prev_delta, curr_out, curr_delta);
    }

    template<typename Optimizer>
    void update_weights(Optimizer& optimizer) {
        if constexpr (detail::has_weight_and_delta_v<derived_type>) {
            update_weights(optimizer, wdeltas, derived().dw, derived().w);
        }
        if constexpr (detail::has_bias_and_delta_v<derived_type>) {
            update_weights(optimizer, bdeltas, derived().db, derived().b);
        }
        clear_gradients();
    }

    /**
     * \brief Helper funciton which combines delta of the layer into a combined
     *        state which then applies the combined delta to the updating tensor
     * \tparam optimizer the Optimizing strategy
     * \tparam combined the tensor to stored the combined delta state of all samples
     * \tparam updating the sensor that is to be updated via the optimizer
     */
    template<typename Optimizer, typename CombiningTensor, typename FromTensor, typename ToTensor>
    void update_weights(Optimizer& optimizer, CombiningTensor& combined, FromTensor& delta, ToTensor& updating) {
        const size_t samples = delta.dimension(0);
        BOOST_ASSERT(samples >= 1);
        /* Sum*/
        combined = delta.chip(0, 0);
        for(size_t s = 1; s < samples; ++s) {
            /* add the other delta chips */
            combined += delta.chip(s, 0);
        }
        if(samples > 1) {
            const float_t reciprocal_batch_size = 1.0f / static_cast<float_t>(samples);
            combined = combined * reciprocal_batch_size;
        };
        optimizer.template update(combined, updating);
    }

    void clear_gradients() {
        if constexpr (detail::has_weight_and_delta_v<derived_type>) {
            derived().dw.setZero();
        }
        if constexpr (detail::has_bias_and_delta_v<derived_type>) {
            derived().db.setZero();
        }
    }

    /**
     * \brief Configures layer. Must be called before use. It is undefined
     *        behavior not to configure a layer before use.
     */
    void configure(const size_t& batch_size, bool reset = false) {
        if constexpr(configures_itself) {
            derived().configuration_impl(batch_size, reset);
        } else {
            default_configuration(batch_size, reset);
        }
    }

    /**
     * \brief Default configuration behavior
     * Can be called from a custom configure_impl in order to keep original default
     * configuration, or alternatively called
     */
    void default_configuration(const size_t& batch_size, bool reset = false) {
        if constexpr(detail::has_weight_and_delta_v<derived_type>) {
            detail::prepare_weights<typename derived_type::weights_dims>(derived().w);
            detail::prepare_and_zero_delta_weights<typename derived_type::weights_dims>(batch_size, derived().dw);
            if(reset) {
                detail::apply_weight_initializer(derived());
            }
        }
        if constexpr(detail::has_bias_and_delta_v<derived_type>) {
            detail::prepare_bias<typename derived_type::output_dims>(derived().b);
            detail::prepare_and_zero_bias_delta<typename derived_type::output_dims>(batch_size, derived().db);
            if(reset) {
                detail::apply_bias_initializer(derived());
            }
        }
        /* Always clear gradients */
        clear_gradients();
    }

    /**
     * Load the layer's weights from the input stream
     * @param is
     */
    void load(std::istream& is) {
        if constexpr(detail::has_weight_and_delta_v<derived_type>) {
            auto& w = derived().w;
            float_t tmp;
            for(size_t i = 0, len = w.size(); i < len; ++i) {
                is >> tmp;
                w.data()[i] = tmp;
            }
        }
        if constexpr(detail::has_bias_and_delta_v<derived_type>) {
            auto& b = derived().b;
            float_t tmp;
            for(size_t i = 0, len = b.size(); i < len; ++i) {
                is >> tmp;
                b.data()[i] = tmp;
            }
        }
    }

    /**
     * Save the layer's weights to the output stream
     * @param os
     */
    void save(std::ostream& os) {
        if constexpr(detail::has_weight_and_delta_v<derived_type>) {
            os << derived().w;
        }
        if constexpr(detail::has_bias_and_delta_v<derived_type>) {
            os << derived().b;
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
     * The target range of the layer for training purposes
     * @return
     */
    valid_range target_range() {
        return {0.0f, 1.0f};
    }

    /**
     * Whether or not a layer is trainable
     */
    bool is_trainable;

    /**
     * \brief Temporarily used in combine_grads
     */
    tensor_4 wdeltas;

    /**
     * \brief Temporarily used in combine_grads
     */
    tensor_1 bdeltas;

    /**
     * \brief Customization point for controlling weight initialization strategy of this layer
     */
    std::function<void(float_t*, float_t*, const size_t&, const size_t&)> weight_initializer;
    /**
     * \brief Customization point for controlling bias initializaiton strategy of this layer
     */
    std::function<void(float_t*, float_t*, const size_t&, const size_t&)> bias_initializer;
};

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_LAYER_HPP */

