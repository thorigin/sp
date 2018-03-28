/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_POOLING_LAYER_MAX_HPP
#define SP_ALGO_NN_LAYER_POOLING_LAYER_MAX_HPP

#include "op.hpp"
#include "layer.hpp"

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Max pooling algorithm
 */
struct max_pooling_algorithm : pooling_algorithm<max_pooling_op, max_pooling_algorithm> {

    template<typename InputDims, typename OutputDims, typename KernelParams>
    void prepare(const size_t& samples) {
        max.resize(samples, InputDims::d, InputDims::h, InputDims::w);
        is_max.resize(samples, InputDims::d, InputDims::h, InputDims::w);
        is_max.setConstant(false);
    }

    inline auto subsample(        max_pooling_op& op,
                                    const size_t& s,
                                    const size_t& d,
                                    const size_t& y,
                                    const size_t& x) {
        /**
         * Store the max value in max at (si, d, max_y, max_x) to
         * a tuple of (y, x) (the input origination)
         */
        max(s, d, op.input_idx[2], op.input_idx[3]) = { y, x };
        /**
         * Set the maximum value
         */
        is_max(s, d, op.input_idx[2], op.input_idx[3]) = true;
        return op.result();
    }

    inline auto upsample(   current_delta_type& curr_delta,
                            const size_t& s,
                            const size_t& d,
                            const size_t& y,
                            const size_t& x) {
        /**
         * Upsample to the above current delta coordinates
         */         
        
        if(is_max(s, d, y, x)) {
            auto [max_y, max_x] = max(s, d, y, x);
            return curr_delta(s, d, max_y, max_x);
        } else {
            return 0.0f;
        }
    }

    /**
     * Tensor, rank 4, (si, od, oy, ox), stored tuple of (y, x)
     */
    tensor_n<4, std::tuple<size_t, size_t>> max;
    tensor_n<4, bool> is_max;
};


/**
 * \brief Max Pooling Layer (alias)
 */
template<
    typename InputVolume,
    typename KernelParams = kernel_params_default,
    typename Connectivity = full_connectivity
>
using max_pooling_layer = pooling_layer<
    max_pooling_algorithm,
    InputVolume,
    KernelParams,
    Connectivity
>;


SP_ALGO_GEN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_POOLING_LAYER_MAX_HPP */

