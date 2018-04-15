/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_ACTIVATION_LAYER_DETAIL_LAYER_HPP
#define SP_ALGO_NN_LAYER_ACTIVATION_LAYER_DETAIL_LAYER_HPP

#include <type_traits>
#include "../op.hpp"
#include "../../detail/layers.hpp"


/**
 * @file Implementation details for applying activation operations
 *
 *
 * Implements specific handling for activation_op_category:
 * - activation_op_unary_category
 * - activation_op_depth_slice_category
 */

SP_ALGO_NN_DETAIL_NAMESPACE_BEGIN

/**
 * \brief Activation operator helper struct
 *
 * Applies an activation operator to the input for forward propagation
 * and applies the activation's derivative to the output gradient for
 * back propagation.
 */
template<typename Op, typename InputDims, typename Enable = void>
struct activation_op_helper;

template<typename Op, typename InputDims>
struct activation_op_helper<
    Op,
    InputDims,
    std::enable_if_t<
        std::is_same_v<
            typename Op::category,
            activation_op_unary_category
        >
    >
> {

    using input_dims = InputDims;
    using op_type = Op;
    using op_deriv_type = typename Op::derivative;

    void fprop(tensor_4& input, tensor_4& output) {
        /* Number of samples in the input */
        const size_t samples = input.dimension(0);
        #pragma omp parallel for simd
        for(size_t si = 0; si < samples; ++si) {
            for(size_t id = 0; id < input_dims::d; ++id)  {
                for (size_t ih = 0; ih < input_dims::h; ++ih) {
                    for (size_t iw = 0; iw < input_dims::w; ++iw) {
                        output(si, id, ih, iw) = op( input(si, id, ih, iw) );
                    }
                }
            }
        }
    }

    void bprop(     tensor_4& prev_out,
                    tensor_4& prev_delta,
                    tensor_4& curr_out,
                    tensor_4& curr_delta) {
        /**
         * Number of samples in the previous output
         */
        const size_t samples = prev_out.dimension(0);
        #pragma omp parallel for simd
        for(size_t si = 0; si < samples; ++si) {
            for(size_t id = 0; id < input_dims::d; ++id)  {
                for (size_t iy = 0; iy < input_dims::h; ++iy) {
                    for (size_t ix = 0; ix < input_dims::w; ++ix) {
                       prev_delta(si, id, iy, ix) = curr_delta(si, id, iy, ix) * op_deriv(curr_out(si, id, iy, ix));
                   }
               }
            }
        }
    }

    op_type op;
    op_deriv_type op_deriv;
};


template<typename Op, typename InputDims>
struct activation_op_helper<
    Op,
    InputDims,
    std::enable_if_t<
        std::is_same_v<
            typename Op::category,
            activation_op_depth_slice_category
        >
    >
> {

    using input_dims = InputDims;
    using op_type = Op;
    using op_deriv_type = typename Op::derivative;

    void fprop(tensor_4& input, tensor_4& output) {
        /* Number of samples in the input */
        const size_t samples = input.dimension(0);
        #pragma omp parallel for simd
        for(size_t si = 0; si < samples; ++si) {
            throw std::runtime_error("Not implemented yet");
        }
    }

    void bprop(     tensor_4& prev_out,
                    tensor_4& prev_delta,
                    tensor_4& curr_out,
                    tensor_4& curr_delta) {
        /**
         * Number of samples in the previous output
         */
        const size_t samples = prev_out.dimension(0);
        #pragma omp parallel for simd
        for(size_t si = 0; si < samples; ++si) {
            throw std::runtime_error("Not implemented yet");
        }
    }

    op_type op;
    op_deriv_type op_deriv;
};


SP_ALGO_NN_DETAIL_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_ACTIVATION_LAYER_DETAIL_LAYER_HPP */

