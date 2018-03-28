/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_CONV_HPP
#define SP_ALGO_NN_LAYER_CONV_HPP

#include <boost/assert.hpp>

#include "layer.hpp"
#include "connectivity.hpp"
#include "detail/layers.hpp"
#include "sp/util/types.hpp"
#include "params.hpp"


SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Convolutional Layer implementation
 *
 * Convolves input by a set of feature kernels and returns the convolution
 * results
 *
 * Summary of parameters (Soft and Hard, i.e. parameterized and derived)
 *  - N - Number of images in a mini-batch
 *  - C - The number of input feature maps
 *  - H - Height of input image
 *  - W - Width of input image
 *  - K - Number of output feature maps (number of kernels)
 *  - R - Height of filter kernel
 *  - S - Width of filter kernel
 *  - U - Vertical Stride
 *  - V - Horizontal Stride
 *  - pad_h - Height of zero-padding
 *  - pad_w - Width of zero-padding
 *
 * Output O of is a four dimensional tensor in R^NKPQ,
 *  where ```P = f(H, R, u, pad_h) and Q = f(W, S, v, pad_w)```
 *  where
 *  *   ```f(H, R, u, pad_h) = ceil((H - R + 1 + 2 * pad_h) / 2)```
 *  *   ```f(W, S, v, pad_w) = ceil((W - S + 1 + 2 * pad_w) / 2)```
 *
 * Convolution Modes (common LA terms, matlab, octave)
 *  - valid - ```pad_w = pad_h = 0```
 *  - same  - ```pad_h = R/2 and pad_w = S / 2```
 *  - full  - ```pad_h = R - 1, pad_w = S - 1```
 *
 * \tparam InputDim, includes:
 * - Width
 * - Height
 * - Depth (or channels)
 * \tparam KernelDim, includes:... tbd
 * \tparam Biased (optional) default true. Whether or not the layer contains bias.
 * \tparam Sparsity The sparsity object. See #group_sparsity and #no_sparsity
 */
template<
    typename InputVolume,
    typename KernelParams = kernel_params_default,
    bool Biased = true,
    typename Connectivity = full_connectivity,
    size_t Dilation = 0
>
struct conv_layer : layer<
    InputVolume,
    detail::convolution_kernel_out_dims_t<InputVolume, KernelParams>,
    conv_layer<InputVolume, KernelParams, Biased, Connectivity, Dilation>
> {

    /**
     * Base type
     */
    using base = layer<
        InputVolume,
        detail::convolution_kernel_out_dims_t<InputVolume, KernelParams>,
        conv_layer<InputVolume, KernelParams, Biased, Connectivity, Dilation>
    >;

    /**
     * Validates KernelParams type
     */
    static_assert(util::is_instantiation_of_v<KernelParams, kernel_params>, "KernelParams template parameter must be an instance of kernel_params");

    /**
     * Kernel parameters
     */
    using kernel_params = KernelParams;

    /**
     * Connectivity type (full, ngroups, etc)
     */
    using connectivity_type = Connectivity;

    /**
     * \brief Dilation parameter
     * \todo TBD: https://arxiv.org/abs/1511.07122
     */
    static_assert(Dilation == 0, "Dilation is not implemented");

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
        kernel_params::h,
        kernel_params::w
    >;

    conv_layer()
        : w(), dw(), b(), db(), connections() {
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
            /**
             * Loop over the pairs of (D_out, D_in), i.e. input_dims::d*output_dims::d
             */
            for (size_t od = 0; od < output_dims::d; ++od) { //D_out
                for (size_t id = 0; id < input_dims::d; ++id) { //D_in
                    if(connections(id, od)) {
                        /*
                         * If the output channel is connected to the input channel,
                         * then perform convolution. This is done to support limited
                         * connectivity when required
                         *
                         * Then, perform the convolution op
                         *
                         * \todo add dilation
                         * \todo Optimize for smaller kernels (common, 2x2, 3x3, etc)
                         */
                        constexpr size_t in_y_len = input_dims::h-output_dims::h;
                        constexpr size_t in_x_len = input_dims::w-output_dims::w;
                        for (size_t iny = 0, ds_y = 0; iny < in_y_len; iny += kernel_params::s_h, ++ds_y) {
                            for (size_t inx = 0, ds_x =  0; inx < in_x_len; inx += kernel_params::s_w, ++ds_x) {
                                float_t sum = 0;
                                for (size_t ky = 0; ky < kernel_params::h; ++ky) {
                                    for (size_t kx = 0; kx < kernel_params::w; ++kx) {
                                        auto in_val = input(si, id, iny+ky, inx+kx);
                                        auto w_val = w(id, od, ky, kx);
                                        sum += in_val * w_val;
                                    }
                                }
                                output(si, od, ds_y, ds_x) += sum;
                            }
                        }
                    }
                }
                if constexpr(biased) {
                    /**
                     * Add bias to every output vector the depth slice at output(od)
                     */
                    using array_2d = std::array<int, 2>;
                    output.chip(si, 0).chip(od, 0) +=
                         b
                        .chip(od, 0) // chip off output depth into a scalar tensor
                        .reshape(array_2d{1, 1}) // reshape to 1x1
                        .broadcast(array_2d{output_dims::h, output_dims::w}); // broadcast to match depth_slice
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
        for(size_t si = 0; si < samples; ++si) {
            /**
             * For every (input depth, output depth) pair that is connected
             */ 
            for (size_t id = 0; id < input_dims::d; ++id) { //D_in
                for (size_t od = 0; od < output_dims::d; ++od) { //D_out
                    if(connections(id, od)) {
                        /**
                         * Propagate the current delta to the previous delta through the kernel
                         */
                        for (size_t oy = 0; oy < output_dims::h; ++oy) {
                            for (size_t ox   = 0; ox < output_dims::w; ++ox) {
                                /**
                                 * Current delta 
                                 */
                                float_t dsrc = curr_delta(si, od, oy, ox);

                                for (size_t wy = 0; wy < weights_dims::h; ++wy) {
                                    for (size_t wx = 0; wx < weights_dims::w; ++wx) {
                                        auto w_val = w(id, od, wy, wx);
                                        auto pdy = oy * kernel_params::s_h + wy;
                                        auto pdx = ox * kernel_params::s_w + wx ;
                                        prev_delta(si, id, pdy, pdx) += w_val * dsrc;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            for (size_t id = 0; id < input_dims::d; ++id) { //D_in
                for (size_t od = 0; od < output_dims::d; ++od) { //D_out
                    if(connections(id, od)) {
                        for (size_t wy = 0; wy < weights_dims::h; ++wy) {
                            for (size_t wx = 0; wx < weights_dims::w; ++wx) {
                                float_t delta = 0;
                                for (size_t oy = 0; oy < output_dims::h; ++oy) {
                                    for (size_t ox = 0; ox < output_dims::w; ++ox) {
                                        auto poy = oy * kernel_params::s_h + wy;
                                        auto pox = ox * kernel_params::s_w + wx;                                        
                                        auto po = prev_out(si, id, poy, pox);                                        
                                        auto cd = curr_delta(si, od, oy, ox);
                                        delta +=  po * cd;                                                                                
                                    }
                                }
                                dw(si, id, od, wy, wx) += delta;
                            }
                        }
                    }
                }
            }
            if (biased) {
                for (size_t od = 0; od < output_dims::d; ++od) {
                    tensor_0 sum = curr_delta.chip(si, 0).chip(od, 0).sum();
                    db(si, od) += sum(0);
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

    connectivity_type connections;

};


SP_ALGO_GEN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_CONV_HPP */

