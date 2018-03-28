/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_ACTIVATION_LAYER_HPP
#define SP_ALGO_NN_LAYER_ACTIVATION_LAYER_HPP

#include "../layer.hpp"
#include "detail/layer.hpp"

/**
 * @file The generic activation layer implementation
 */

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Activation Layer
 *
 * Considered an add on operation for layer operations, takes the output of the
 * previous layer and performs activation (and derivative) for fprop and bprop
 *
 * \tparam InputVolume the size of the input volume, also will be the output
 *         size
 * \tparam ActivationOp the activation operator which will be used to used for
 *         back propagation. Two types allowed:
 *          - UnaryActivationOp, performs activation on a per output value basis
 *          - DepthSliceActivationOp, performs activation on depth output value basis
 */
template<typename InputVolume, typename ActivationOp>
struct activation_layer : layer<
    InputVolume,
    InputVolume,
    activation_layer<
        InputVolume,
        ActivationOp
    >,
    false
> {

    using base = layer<
        InputVolume,
        InputVolume,
        activation_layer<
            InputVolume,
            InputVolume
        >
    >;

    /**
     * Activation op type
     */
    using activation_op_type = ActivationOp;

    /**
     * The derivative of the activation type
     */
    using activation_op_deriv_type = typename ActivationOp::derivative;

    /**
     * \brief Input Dimensions
     */
    using input_dims = typename base::input_dims;

    /**
     * \brief Output Dimensions
     */
    using output_dims = typename base::output_dims;

    void forward_prop_impl(input_type& input, output_type& output) {
        detail::activation_op_helper<
            activation_op_type,
            input_dims
        >().fprop(input, output);
    }

    void backward_prop_impl(    previous_output_type& prev_out,
                                previous_delta_type& prev_delta,
                                current_type& curr_out,
                                current_delta_type& curr_delta) {
        detail::activation_op_helper<
            activation_op_type,
            input_dims
        >().bprop(prev_out, prev_delta, curr_out, curr_delta);
    }


    valid_range range_impl() {
        return activation_op_type().range();
    }
    
};
        
SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_ACTIVATION_LAYER_HPP */

