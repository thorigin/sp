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
#include "../../weight.hpp"


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
    void configure(const size_t& samples) {
        derived().template configure_impl<InputDims, OutputDims, KernelParams>(samples);
    }

    template<typename InputDims, typename OutputDims, typename KernelParams>
    void before_forward(const size_t& samples) {
        derived().template before_forward_impl<InputDims, OutputDims, KernelParams>(samples);
    }

    sp_hot auto subsample(          op_type& op,
                                    const size_t& s,
                                    const size_t& d,
                                    const size_t& y,
                                    const size_t& x) {
        return derived().subsample_impl(op, s, d, y, x);
    }

    /**
     * Return the upsampled value
     */
    sp_hot auto upsample(   tensor_4& curr_delta,
                            const size_t& si,
                            const size_t& od,
                            const size_t& iy,
                            const size_t& ix) {
        return derived().upsample_impl(curr_delta, si, od, iy, ix);
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
    bool Biased = true
>
struct pooling_layer : layer<
    InputVolume,
    detail::pooling_kernel_out_dims_t<InputVolume, KernelParams>,
    pooling_layer<PoolingAlgorithm, InputVolume, KernelParams>,
    true,
    true
> {

    /**
     * Base type
     */
    using base = layer<
        InputVolume,
        detail::pooling_kernel_out_dims_t<InputVolume, KernelParams>,
        pooling_layer<PoolingAlgorithm, InputVolume, KernelParams>,
        true,
        true
    >;

    /**
     * Validates KernelParams type
     */
    static_assert(util::is_instantiation_of_v<KernelParams, kernel_params>, "KernelParams template parameter must be an instance of kernel_params");

    constexpr static bool biased = Biased;

    /**
     * Kernel parameters
     */
    using kernel_params = KernelParams;

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
    using weights_dims = weight_dims<output_dims::d>;

    using pooling_algorithm_type = PoolingAlgorithm;

    using down_sampler_op_type = typename PoolingAlgorithm::op_type;

    pooling_layer() {
        /**
         * Default to a fixed weight initializer as network default is not
         * appropriate for pooling operations
         */
        this->weight_initializer = fixed_weight_initializer(1);
        this->bias_initializer = fixed_weight_initializer(0);
    }

    void forward_prop_impl(tensor_4& input, tensor_4& output) {
        /**
         * Number of samples in the input
         */
        const size_t samples = input.dimension(0);

        pooling_algorithm.template before_forward<input_dims, output_dims, kernel_params>(samples);

        #pragma omp parallel for simd
        for (size_t si = 0; si < samples; ++si) {
            for (size_t od = 0; od < output_dims::d; ++od) {
                auto& weight = w(od, 0, 0, 0);
                auto& bias = b(od);
                for (size_t oy = 0, iny = 0; oy < output_dims::h; ++oy, iny += kernel_params::s_h) {
                    for (size_t ox = 0, inx = 0; ox < output_dims::w; ++ox, inx += kernel_params::s_w) {
                        down_sampler_op_type op(kernel_params::h * kernel_params::w);
                        for (size_t ky = 0; ky < kernel_params::h; ++ky) {
                            for (size_t kx = 0; kx < kernel_params::w; ++kx) {
                                op.sample(input, si, od, iny+ky, inx+kx);
                            }
                        }
                        float_t res = pooling_algorithm.subsample(op, si, od, oy, ox);
                        res *= weight;
                        res += bias;
                        output(si, od, oy, ox) = res;
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
    void backward_prop_impl(    tensor_4& prev_out,
                                tensor_4& prev_delta,
                                tensor_4& curr_out,
                                tensor_4& curr_delta) {
        /**
         * Number of samples in the input
         */
        const size_t samples = prev_out.dimension(0);

        #pragma omp parallel for simd
        for (size_t si = 0; si < samples; ++si) {
            for (size_t d = 0; d < output_dims::d; ++d) {
                auto& weight = w(d, 0, 0, 0);
                for (size_t iy = 0; iy < input_dims::h; ++iy) {
                    for (size_t ix = 0; ix < input_dims::w; ++ix) {
                        /* Upsample the value from output delta */
                        auto upsampled_cd = pooling_algorithm.upsample(curr_delta, si, d, iy, ix);
                        prev_delta(si, d, iy, ix) += weight * upsampled_cd;
                        dw(si, d, 0, 0, 0) += prev_out(si, d, iy, ix) * upsampled_cd;
                    }
                }

                for (size_t oy = 0; oy < output_dims::h; ++oy) {
                    for (size_t ox = 0; ox < output_dims::w; ++ox) {
                        db(si, d) += curr_delta(si, d, oy, ox);
                    }
                }
            }
        }
    }

    void configuration_impl(const size_t& batch_size, bool reset) {
        this->default_configuration(batch_size, reset);
        pooling_algorithm.template configure<input_dims, output_dims, kernel_params>(batch_size);
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

    pooling_algorithm_type pooling_algorithm;

};


SP_ALGO_GEN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_POOLING_LAYER_HPP */

