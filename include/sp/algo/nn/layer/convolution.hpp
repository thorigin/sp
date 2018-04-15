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
 * Note: This convolution implementation does not rotate the kernel
 *       and is therefore not convolution but cross correlation, mathematically
 *       speaking.
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

    void forward_prop_impl(tensor_4& input, tensor_4& output) {

        /**
         * Number of samples in the input
         */
        const size_t samples = input.dimension(0);

        /**
         * Perform  forward propagation for every sample
         */
        #pragma omp parallel for simd
        for (size_t si = 0; si < samples; ++si) {
            /**
             * Loop over the pairs of (D_out, D_in), i.e. input_dims::d*output_dims::d
             */
            for (size_t od = 0; od < output_dims::d; ++od) {
                for (size_t id = 0; id < input_dims::d; ++id) {
                    if(connections(od, id)) {
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
                        for (size_t oy = 0, iny = 0; oy < output_dims::h; ++oy, iny += kernel_params::s_h) {
                            for (size_t ox = 0, inx = 0; ox < output_dims::w; ++ox, inx += kernel_params::s_w) {
                                float_t sum = 0;
                                for (size_t ky = 0; ky < kernel_params::h; ++ky) {
                                    for (size_t kx = 0; kx < kernel_params::w; ++kx) {
                                        auto& in_val = input(si, id, iny+ky, inx+kx);
                                        auto& w_val = w(od, id, ky, kx);
                                        sum += in_val * w_val;
                                    }
                                }
                                output(si, od, oy, ox) += sum;
                            }
                        }
                    }
                }
                if constexpr(biased) {
                    /**
                     * Add bias to every output vector the depth slice at output(od)
                     */
                    output.chip(si, 0).chip(od, 0) = output.chip(si, 0).chip(od, 0) + b(od);
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
                                tensor_4& curr_delta)  {


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
        for(size_t si = 0; si < samples; ++si) {
            /**
             * For every (input depth, output depth) pair that is connected
             */
            for (size_t id = 0; id < input_dims::d; ++id) {
                for (size_t od = 0; od < output_dims::d; ++od) {
                    if(connections(od, id)) {
                        /* Propagate the current delta to the previous delta through the kernel */
                        for (size_t oy = 0, iny = 0; oy < output_dims::h; ++oy, iny += kernel_params::s_h) {
                            for (size_t ox = 0, inx = 0; ox < output_dims::w; ++ox, inx += kernel_params::s_w) {
                                float_t& grad = curr_delta(si, od, oy, ox);
                                for (size_t wy = 0; wy < weights_dims::h; ++wy) {
                                    for (size_t wx = 0; wx < weights_dims::w; ++wx) {
                                        auto& w_val = w(od, id, wy, wx);
                                        prev_delta(si, id, iny + wy, inx + wx) += w_val * grad;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            for (size_t id = 0; id < input_dims::d; ++id) {
                for (size_t od = 0; od < output_dims::d; ++od) {
                    if(connections(od, id)) {
                        for (size_t wy = 0; wy < weights_dims::h; ++wy) {
                            for (size_t wx = 0; wx < weights_dims::w; ++wx) {
                                float_t delta = 0;
                                for (size_t oy = 0, iny = 0; oy < output_dims::h; ++oy, iny += kernel_params::s_h) {
                                    for (size_t ox = 0, inx = 0; ox < output_dims::w; ++ox, inx += kernel_params::s_w) {
                                        auto& po = prev_out(si, id, oy + wy, ox + wx);
                                        auto& cd = curr_delta(si, od, oy, ox);
                                        delta +=  po * cd;
                                    }
                                }
                                dw(si, od, id, wy, wx) += delta;
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
     */
    weights_type w;

    /**
     * Weights Delta
     */
    weights_delta_type dw;

    /**
     * \brief Bias.
     */
    bias_type b;

    /**
     * Bias Delta.
     */
    bias_delta_type db;

    connectivity_type connections;

};


SP_ALGO_GEN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_CONV_HPP */

