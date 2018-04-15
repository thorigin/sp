/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_POOLING_LAYER_AVG_HPP
#define SP_ALGO_NN_LAYER_POOLING_LAYER_AVG_HPP

#include "op.hpp"
#include "layer.hpp"
#include "sp/util/hints.hpp"

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Average pooling algorithm
 */
struct mean_pooling_algorithm : pooling_algorithm<mean_pooling_op, mean_pooling_algorithm> {

    template<typename InputDims, typename OutputDims, typename KernelParams>
    void configure_impl(const size_t&) {
        stride_h = KernelParams::s_h;
        stride_w = KernelParams::s_w;
        upsample_scaling = float_t(1.0) / (KernelParams::w * KernelParams::h);
    }

    template<typename InputDims, typename OutputDims, typename KernelParams>
    void before_forward_impl(const size_t&) {}

    sp_hot auto subsample_impl(     mean_pooling_op& op,
                                    const size_t&,
                                    const size_t&,
                                    const size_t&,
                                    const size_t&) {
        return op.result();
    }

    sp_hot auto upsample_impl(   tensor_4& curr_delta,
                            const size_t& s,
                            const size_t& d,
                            const size_t& y,
                            const size_t& x) {
        return curr_delta(s, d, y / stride_h , x / stride_w) * upsample_scaling;
    }

    size_t stride_h, stride_w;
    float_t upsample_scaling;
};


/**
 * \brief Mean Pooling Layer (alias)
 */
template<
    typename InputVolume,
    typename KernelParams = kernel_params_default,
    bool Biased = true
>
using mean_pooling_layer = pooling_layer<
    mean_pooling_algorithm,
    InputVolume,
    KernelParams,
    Biased
>;

SP_ALGO_GEN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_POOLING_LAYER_AVG_HPP */

