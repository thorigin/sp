/**
 * Copyright (C) Omar Thor <omarthoro\gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro\gmail.com>, 2017
 */
#ifndef SP_ALGO_NN_LAYER_FULLY_CONNECTED_HPP
#define SP_ALGO_NN_LAYER_FULLY_CONNECTED_HPP

#include "layer.hpp"
#include "activation.hpp"

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Fully Connected Layer, also known as an inner product layer.
 */
template<
    typename InputDims,
    size_t OutputSize,
    bool Biased = true
>
struct fully_connected_layer : layer<
    InputDims,
    volume_dims<
        OutputSize
    >,
    fully_connected_layer<InputDims, OutputSize, Biased>
> {

    /**
     * Base
     */
    using base = layer<
        InputDims,
        volume_dims<
            OutputSize
        >,
        fully_connected_layer<InputDims, OutputSize, Biased>
    >;

    /**
     * Whether or not this layer has biased
     */
    constexpr static bool biased = Biased;

    /**
     * \brief Input Dimensions
     */
    using input_dims = typename base::input_dims;

    /**
     * \brief Output Dimensions
     */
    using output_dims = typename base::output_dims;

    /**
     * \brief Weight dimensions
     */
    using weights_dims = weight_dims<
        output_dims::d,
        input_dims::d,
        input_dims::w,
        input_dims::h
    >;

    fully_connected_layer() : w(), dw(), b(), db() {
        detail::init_weights<weights_dims>(w);
        detail::init_weights_delta<weights_dims>(1, dw);
        if constexpr (biased) {
            detail::init_bias<output_dims::d>(b);
        }
    }


    void forward_prop_impl(input_type& input, output_type& output) {

        /**
         * Number of samples in the input
         */
        const size_t samples = input.dimension(0);

        /**
         * Initialize the output to match the number of samples
         * and the output dimensions
         */
        detail::prepare_output<output_dims>(samples, output);

        /**
         * Perform  forward propagation for every sample
         */
        for (size_t si = 0; si < samples; ++si) { // K
            for(size_t o = 0; o < output_dims::size; ++o) {
                for (size_t id = 0; id < input_dims::d; ++id) { //D_in
                    for (size_t iy = 0; iy < input_dims::h; ++iy) {
                        for (size_t ix = 0; ix < input_dims::w; ++ix) {
                            output(si, o, 0, 0) += input(si, id, iy, ix) * w(o, id, iy, ix);
                        }
                    }
                }
                if constexpr(biased) {
                    output(si, o, 0, 0) += b(o);
                }
            }
        }
    }

    /**
     * \brief Back propagation implementation
     *
     * \todo Optimize
     */
    void backward_prop_impl(    previous_output_type& prev_out,
                                previous_delta_type& prev_delta,
                                current_type& curr_out,
                                current_delta_type& curr_delta)  { //curr_delta


        /**
         * Number of samples in the previous output
         */
        const size_t samples = prev_out.dimension(0);

        /**
         * @TODO the following prepare statements should perhaps be handled by
         * the network layer
         */

        /**
         * Initialize the previous delta to match the number of samples in the
         * current delta
         */
        detail::prepare_output<input_dims>(samples, prev_delta);

        /**
         * Ensure that bias can handle the sample delta
         */
        detail::prepare_bias_delta<output_dims>(samples, db);

        /**
         * Initialize weight delta
         */
        detail::prepare_delta_weights<weights_dims>(samples, dw);

        /**
         * Perform back propagation
         *
         * For every sample in the input
         */
        for (size_t si = 0; si < samples; ++si) { // K
            for(size_t o = 0; o < output_dims::size; ++o) {
                for (size_t id = 0; id < input_dims::d; ++id) { //D_in
                    for (size_t iy = 0; iy < input_dims::h; ++iy) {
                        for (size_t ix = 0; ix < input_dims::w; ++ix) {
                            prev_delta(si, id, iy, ix) += curr_delta(si, o, 0, 0) * w(o, id, iy, ix);
                            dw(si, o, id, iy, ix) += curr_delta(si, o, 0, 0) * prev_out(si, id, iy, ix);
                        }
                    }
                }
                if constexpr(biased) {
                    db(o) += curr_delta(si, o, 0, 0);
                }
            }
        }
    }

    /**
     * \brief Weights of the layer.
     *
     * Size is (D_out, D_in, H_kernel, W_Kernel)
     */
    weights_type w;

    /**
     * Weights Delta
     *
     * Size is (Sample, D_Out, D_in, H_kernel, W_Kernel)
     */
    weights_delta_type dw;

    /**
     * \brief Bias.
     *
     * Size is (D_out)
     */
    bias_type b;

    /**
     * Bias Delta.
     *
     * Size is (Sample, D_out)
     */
    bias_delta_type db;

};

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_FULLY_CONNECTED_HPP */

