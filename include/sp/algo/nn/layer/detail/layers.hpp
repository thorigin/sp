/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_DETAIL_LAYERS_HPP
#define SP_ALGO_NN_LAYER_DETAIL_LAYERS_HPP

#include "sp/util/types.hpp"
#include "../params.hpp"
#include "../../types.hpp"
#include "../../matrix.hpp"

SP_ALGO_NN_DETAIL_NAMESPACE_BEGIN

/**
 * \brief Calculate the padded size of an input when using a kernel
 * 
 * Modifies, if necessary, the input dimension
 */
template<typename InputDim, typename KernelParams>
using apply_padding_t = std::conditional_t<
    KernelParams::padding = padding_type::same,
    volume_dims<
        InputDim::d,
        InputDim::w + KernelParams::w - 1,
        InputDim::h + KernelParams::h - 1
    >,
    InputDim
>;

/**
 * \brief Kernel output size, derived from Input dimensions and kernel
 *        parameters
 */
template<typename InputDim, typename KernelParams>
using convolution_kernel_out_dims_t = volume_dims<
    KernelParams::d,
    (InputDim::w - KernelParams::w + 0)/KernelParams::s_w + 1,
    (InputDim::h - KernelParams::h + 0)/KernelParams::s_h + 1
>;

/**
 * \brief Kernel output size, derived from Input dimensions and kernel
 *        parameters
 */
template<typename InputDim, typename KernelParams>
using pooling_kernel_out_dims_t = volume_dims<
    InputDim::d,
    (InputDim::w - KernelParams::w + 0)/KernelParams::s_w + 1,
    (InputDim::h - KernelParams::h + 0)/KernelParams::s_h + 1
>;

/**
 * \brief Initializes the output
 */
template<typename OutputDims>
void prepare_output(const size_t& samples, output_type& out) {
    static_assert(util::is_instantiation_of_v<OutputDims, volume_dims>, "OutputDims is an instantiation of volume_dims");
    out.resize(samples, OutputDims::d, OutputDims::h, OutputDims::w);
    out.setZero(); //@TODO remove, debugging purposes
}


/**
 * \brief Initializes the current delta
 */
template<typename OutputDims>
void init_current_delta(const size_t& samples, output_type& out) {
    static_assert(util::is_instantiation_of_v<OutputDims, volume_dims>, "OutputDims is an instantiation of volume_dims");
    out.resize(samples, OutputDims::d, OutputDims::h, OutputDims::w);
    out.setZero(); //@TODO remove, debugging purposes
}



/**
 * \brief Initialize weight sensor to zero
 */
template<typename WeightDims>
void init_weights(tensor_4& w) {
    static_assert(util::is_instantiation_of_v<WeightDims, weight_dims>, "WeightDims is an instantiation of weight_dims");
    tensor_zero<
        WeightDims::out,
        WeightDims::in,
        WeightDims::h,
        WeightDims::w
    >(w);
}

/**
 * \brief Initialize weight sensor to one
 */
template<typename WeightDims>
void init_weights_one(tensor_4& w) {
    static_assert(util::is_instantiation_of_v<WeightDims, weight_dims>, "WeightDims is an instantiation of weight_dims");
    tensor_val<
        WeightDims::out,
        WeightDims::in,
        WeightDims::h,
        WeightDims::w
    >(w, 1.0f);
}

/**
 * \brief Initialize weight sensor to zero
 */
template<typename WeightDims>
void init_weights_delta(const size_t& samples, weights_delta_type& t) {
    static_assert(util::is_instantiation_of_v<WeightDims, weight_dims>, "WeightDims is an instantiation of weight_dims");
    t.resize(
        samples,
        WeightDims::out,
        WeightDims::in,
        WeightDims::h,
        WeightDims::w
    );
    t.setZero();
}


/**
 * \brief Initialize weight sensor to zero
 */
template<size_t size>
void init_bias(bias_type& t) {
    static_assert(size > 0, "Size must be greater thann 0");
    tensor_zero<
        size
    >(t);
}


/**
 * \brief Initialize weight sensor to zero
 */
template<typename OutputDims>
void init_bias_delta(bias_type& t) {
    static_assert(util::is_instantiation_of_v<OutputDims, volume_dims>, "OutputDims is an instantiation of volume_dims");
    tensor_zero<
        1,
        OutputDims::d
    >(t);
}

/**
 * \brief Ensure that bias delta is large enough for sample imput
 *
 * Assumes bias is 1:1 with the number of output dimensions
 */
template<typename OutputDims>
void prepare_bias_delta(const size_t& sample, bias_delta_type& db) {
    static_assert(util::is_instantiation_of_v<OutputDims, volume_dims>, "OutputDims is an instantiation of volume_dims");
    if( static_cast<size_t>(db.dimension(0)) != sample ||
        static_cast<size_t>(db.dimension(1)) != OutputDims::d) {
        db.resize(sample, OutputDims::d);        
    }
    db.setZero();
}

/**
 * \brief Initialize weight sensor to zero
 */
template<typename DeltaWeightDims>
void prepare_delta_weights(const size_t& samples, weights_delta_type& t) {
    static_assert(util::is_instantiation_of_v<DeltaWeightDims, weight_dims>, "DeltaWeightDims is an instantiation of weight_dims");
    t.resize(samples, DeltaWeightDims::in, DeltaWeightDims::out, DeltaWeightDims::h, DeltaWeightDims::w);
    t.setZero();
}

template <typename, typename = void>
struct has_delta_weight_helper : std::false_type {};

template <typename T>
struct has_delta_weight_helper<
    T,
    std::void_t<
        decltype(std::declval<T>().db)
    >
> : std::true_type {};


template <typename, typename = void>
struct has_delta_bias_helper : std::false_type {};

template <typename T>
struct has_delta_bias_helper<
    T,
    std::void_t<
        decltype(std::declval<T>().db)
    >
> : std::true_type {};

/**
 * \brief Check if a layer has delta weight states
 */
template<typename Layer>
constexpr bool has_delta_weight_v = has_delta_weight_helper<Layer>::value;

/**
 * \brief Check if a layer has delta bias states
 */
template<typename Layer>
constexpr bool has_delta_bias_v = has_delta_bias_helper<Layer>::value;


SP_ALGO_NN_DETAIL_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_DETAIL_LAYERS_HPP */

