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
        input_dims::d,
        input_dims::h,
        input_dims::w,
        output_dims::size
    >;

    void forward_prop_impl(tensor_4& input, tensor_4& output) {
        /**
         * Number of samples in the input
         */
        const size_t samples = input.dimension(0);
        /**
         * Perform forward propagation for every sample
         */
        #pragma omp parallel for simd
        for (size_t si = 0; si < samples; ++si) {
            for(size_t od = 0; od < output_dims::size; ++od) {
                for (size_t id = 0; id < input_dims::d; ++id) {
                    for (size_t iy = 0; iy < input_dims::h; ++iy) {
                        for (size_t ix = 0; ix < input_dims::w; ++ix) {
                            output(si, od, 0, 0) += w(id, iy, ix, od) * input(si, id, iy, ix);
                        }
                    }
                }
                if constexpr(biased) {
                    output(si, od, 0, 0) += b(od);
                }
            }
        }
    }

    /**
     * \brief Back propagation implementation
     *
     * \todo Optimize
     */
    void backward_prop_impl(    tensor_4& prev_out,
                                tensor_4& prev_delta,
                                tensor_4& curr_out,
                                tensor_4& curr_delta) {
        /**
         * Number of samples in the previous output
         */
        const size_t samples = prev_out.dimension(0);
        /**
         * Perform back propagation
         *
         * For every sample in the input
         */
        #pragma omp parallel for simd
        for (size_t si = 0; si < samples; ++si) {
            for(size_t od = 0; od < output_dims::size; ++od) {
                auto& grad = curr_delta(si, od, 0, 0);
                for (size_t id = 0; id < input_dims::d; ++id) {
                    for (size_t iy = 0; iy < input_dims::h; ++iy) {
                        for (size_t ix = 0; ix < input_dims::w; ++ix) {
                            auto& w_i = w(id, iy, ix, od);
                            auto& p_out = prev_out(si, id, iy, ix);
                            prev_delta(si, id, iy, ix) += grad * w_i;
                            dw(si, id, iy, ix, od) += grad * p_out;
                        }
                    }
                }
            }
            if constexpr(biased) {
                for(size_t od = 0; od < output_dims::size; ++od ) {
                    db(si, od) += curr_delta(si, od, 0, 0);
                }
            }
        }
    }

    /**
     * \brief Weights of the layer.
     *
     * Size is (D_out, D_in, With, Height)
     */
    weights_type w;

    /**
     * Weights Delta
     *
     * Size is (Sample, D_Out, D_in, In_H, In_W)
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

