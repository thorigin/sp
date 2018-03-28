/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_POOLING_LAYER_HPP
#define SP_ALGO_NN_LAYER_POOLING_LAYER_HPP

#include "../params.hpp"
#include "../layer.hpp"
#include "../detail/layers.hpp"
#include "op.hpp"


SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Pooling Algorithm
 *
 * Provides a pluggable point for extensions of the pooling layer.
 *
 * See mean_pooling_algorithm and max_pooling_algorithm
 */
template<typename PoolingOperator, typename Derived>
struct pooling_algorithm {

    using derived_type = Derived;

    using op_type = PoolingOperator;

    
    template<typename InputDims, typename OutputDims, typename KernelParams>
    void prepare(const size_t& samples) {
        derived().template prepare<InputDims, OutputDims, KernelParams>(samples);
    }

    inline auto subsample(        max_pooling_op& op,
                                    const size_t& s,
                                    const size_t& d,
                                    const size_t& y,
                                    const size_t& x) {
        return derived().down_sample(op, s, d, y, x);
    }

    /**
     * Return the upsampled value
     */
    inline auto upsample(   current_delta_type& curr_delta,
                            const size_t& si,
                            const size_t& od,
                            const size_t& iy,
                            const size_t& ix) {
        return derived().upsample(curr_delta, si, od, iy, ix);
    }

    derived_type& derived() {
        return static_cast<derived_type&>(*this);
    }
};

/**
 * \brief Pooling layer abstract class
 *
 * Down samples a layer using the operator specified
 */
template<
    typename PoolingAlgorithm,
    typename InputVolume,
    typename KernelParams = pooling_kernel_params<>,
    typename Connectivity = full_connectivity
>
struct pooling_layer : layer<
    InputVolume,
    detail::pooling_kernel_out_dims_t<InputVolume, KernelParams>,
    pooling_layer<PoolingAlgorithm, InputVolume, KernelParams, Connectivity>
> {

    /**
     * Base type
     */
    using base = layer<
        InputVolume,
        detail::convolution_kernel_out_dims_t<InputVolume, KernelParams>,
        pooling_layer<PoolingAlgorithm, InputVolume, KernelParams, Connectivity>
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
        1,
        input_dims::d,
        1,
        1
    >;

    using pooling_algorithm_type = PoolingAlgorithm;

    using down_sampler_op_type = typename PoolingAlgorithm::op_type;

    pooling_layer()
        : w(), dw(), b(), db(), connections(), pooling_algorithm() {
        detail::init_weights_one<weights_dims>(w);
        detail::init_weights_delta<weights_dims>(1, dw);
        detail::init_bias<input_dims::d>(b);
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

        pooling_algorithm.template prepare<input_dims, output_dims, kernel_params>(samples);

        for (size_t si = 0; si < samples; ++si) { // K
            for (size_t od = 0; od < output_dims::d; ++od) { //D_out
                auto weight = w(0, od, 0, 0);
                auto bias = b(od);

                for (size_t id = 0; id < input_dims::d; ++id) { //D_in
                    if(connections(id, od)) {
                        
                        for (size_t oy = 0; oy < output_dims::h; ++oy) {
                            for (size_t ox = 0; ox < output_dims::w; ++ox) {
                                down_sampler_op_type op;
                                for (size_t ky = 0; ky < kernel_params::h; ++ky) {
                                    for (size_t kx = 0; kx < kernel_params::w; ++kx) {
                                        auto in_val = input(si, id, oy * kernel_params::s_h + ky, ox* kernel_params::s_w +kx);
                                        op.sample(in_val, si, id, oy * kernel_params::s_h + ky, ox* kernel_params::s_w +kx);
                                    }
                                }
                                float res = pooling_algorithm.subsample(op, si, od, oy, ox);
                                res *= weight;
                                res += bias;                                
                                output(si, od, oy, ox) = res;
                            }
                        }
                    }
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
                                current_delta_type& curr_delta)  {
        /**
         * Number of samples in the input
         */
        const size_t samples = prev_out.dimension(0);

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

        for (size_t si = 0; si < samples; ++si) { // K
            for (size_t od = 0; od < output_dims::d; ++od) { //D_out
                auto weight = w(od, 0, 0, 0);

                for (size_t id = 0; id < input_dims::d; ++id) { //D_in
                    if(connections(id, od)) {                        
                        float_t delta = 0;
                        for (size_t iy = 0; iy < input_dims::h; ++iy) {
                            for (size_t ix = 0; ix < input_dims::w; ++ix) {
                                /**
                                 * Upsample the value from curr_delta at (si, od, iy, ix)
                                 */
                                auto upsampled_cd = pooling_algorithm.upsample(curr_delta, si, od, iy, ix);
                                prev_delta(si, id, iy, ix) = weight * upsampled_cd;
                                delta += prev_out(si, id, iy, ix) * upsampled_cd;
                            }
                        }
                        
                        dw(si, 0, od, 0, 0) += delta;

                        for (size_t oy = 0; oy < output_dims::h; ++oy) {
                            for (size_t ox = 0; ox < output_dims::w; ++ox) {
                                db(si, od) += curr_delta(si, id, oy, ox);
                            }
                        }
                    }
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

    pooling_algorithm_type pooling_algorithm;
    
};


SP_ALGO_GEN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_POOLING_LAYER_HPP */

