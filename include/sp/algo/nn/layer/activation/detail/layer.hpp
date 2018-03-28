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

    void fprop(input_type& input, output_type& output) {
        /* Number of samples in the input */
        const size_t samples = input.dimension(0);
        /* Prepare the output */
        prepare_output<input_dims>(samples, output);
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

    void bprop(     previous_output_type& prev_out,
                    previous_delta_type& prev_delta,
                    current_type& curr_out,
                    current_delta_type& curr_delta) {
        /**
         * Number of samples in the previous output
         */
        const size_t samples = prev_out.dimension(0);

        /* Prepare the output */
        prepare_output<input_dims>(samples, prev_out);

        for(size_t si = 0; si < samples; ++si) {
            for(size_t id = 0; id < input_dims::d; ++id)  {
                for (size_t ih = 0; ih < input_dims::h; ++ih) {
                    for (size_t iw = 0; iw < input_dims::w; ++iw) {
                       prev_delta(si, id, ih, iw) = curr_delta(si, id, ih, iw) * op_deriv(curr_out(si, id, ih, iw));
                   }
               }
            }
        }
    }

    op_type op;
    op_deriv_type op_deriv;
};
//
//template<typename Op, typename InputDims>
//struct activation_op_helper<
//    Op,
//    InputDims,
//    std::enable_if_t<
//        std::is_same_v
//            typename Op::category,
//            activation_op_depth_slice_category
//        >
//    >
//> {
//    using input_dims = InputDims;
//
//    using op_type = Op;
//    using op_deriv_type = Op::deriv;
//
//    void fprop(input_type& input, output_type& output) {
//
//        throw std::runtime_error("Not implemented yet");
//    }
//
//    void bprop(     previous_output_type& prev_out,
//                    previous_delta_type& prev_delta,
//                    current_delta_type& curr_delta) {
//
//        throw std::runtime_error("Not implemented yet");
//    }
//
//    op_type op;
//    op_deriv_type op_deriv;
//};

SP_ALGO_NN_DETAIL_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_ACTIVATION_LAYER_DETAIL_LAYER_HPP */

