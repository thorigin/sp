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
 * \file Contains various layer helpers, initializers, and plugin-points
 */

/**
 * \brief Validate that the tensor matched the volume expected
 *
 * @return true if matching
 */
template<typename VolumeDimsExpected>
bool validate_dimensions(const sample_type& sample) {
    static_assert(util::is_instantiation_of_v<VolumeDimsExpected, volume_dims>, "VolumeDimsExpected is an instantiation of volume_dims");
    using expected = VolumeDimsExpected;
    auto& dims = sample.dimensions();
    return      dims[0] == expected::d &&
                dims[1] == expected::h &&
                dims[2] == expected::w;
}

/**
 * \brief Validate that the tensor matched the volume expected. Note that
 *        it does not consider sample index
 *
 * @return true if matching
 */
template<typename VolumeDimsExpected>
bool validate_dimensions(const tensor_4& t) {
    static_assert(util::is_instantiation_of_v<VolumeDimsExpected, volume_dims>, "VolumeDimsExpected is an instantiation of volume_dims");
    using expected = VolumeDimsExpected;
    auto& dims = t.dimensions();

    return      static_cast<size_t>(dims[1]) == expected::d &&
                static_cast<size_t>(dims[2]) == expected::h &&
                static_cast<size_t>(dims[3]) == expected::w;
}

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
        InputDim::h + KernelParams::h - 1,
        InputDim::w + KernelParams::w - 1
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
    (InputDim::h - KernelParams::h + 0)/KernelParams::s_h + 1,
    (InputDim::w - KernelParams::w + 0)/KernelParams::s_w + 1
>;

/**
 * \brief Kernel output size, derived from Input dimensions and kernel
 *        parameters
 */
template<typename InputDim, typename KernelParams>
using pooling_kernel_out_dims_t = volume_dims<
    InputDim::d,
    (InputDim::h - KernelParams::h + 0)/KernelParams::s_h + 1,
    (InputDim::w - KernelParams::w + 0)/KernelParams::s_w + 1
>;

/**
 * \brief Initializes the output to the specified size and zeroes it
 */
template<typename VolumeDims>
void prepare_tensor(const size_t& samples, tensor_4& tens) {
    static_assert(util::is_instantiation_of_v<VolumeDims, volume_dims>, "VolumeDims is an instantiation of volume_dims");
    tens.resize(samples, VolumeDims::d, VolumeDims::h, VolumeDims::w);
    tens.setZero();
}

/**
 * \brief Initializes the output to the specified size
 */
template<typename VolumeDims>
void prepare_and_zero_tensor(const size_t& samples, tensor_4& tens) {
    static_assert(util::is_instantiation_of_v<VolumeDims, volume_dims>, "VolumeDims is an instantiation of volume_dims");
    tens.resize(samples, VolumeDims::d, VolumeDims::h, VolumeDims::w);
    tens.setZero();
}

/**
 * \brief Initialize weight sensor
 */
template<typename WeightDims>
void prepare_weights(tensor_4& w) {
    static_assert(util::is_instantiation_of_v<WeightDims, weight_dims>, "WeightDims is an instantiation of weight_dims");
    w.resize(
        WeightDims::out,
        WeightDims::in,
        WeightDims::h,
        WeightDims::w
    );
}

/**
 * \brief Ensure that bias is large enough for sample output
 *
 * Assumes bias is 1:1 with the number of output dimensions
 */
template<typename OutputDims>
void prepare_bias(bias_type& t) {
    static_assert(util::is_instantiation_of_v<OutputDims, volume_dims>, "OutputDims is an instantiation of volume_dims");
    t.resize(OutputDims::d);
}

/**
 * \brief Ensure that bias delta is large enough for sample imput
 *
 * Assumes bias is 1:1 with the number of output dimensions
 */
template<typename OutputDims>
void prepare_and_zero_bias_delta(const size_t& batch_size, bias_delta_type& db) {
    static_assert(util::is_instantiation_of_v<OutputDims, volume_dims>, "OutputDims is an instantiation of volume_dims");
    db.resize(batch_size, OutputDims::d);
    db.setZero();
}

/**
 * \brief Initialize weight sensor to zero
 */
template<typename DeltaWeightDims>
void prepare_and_zero_delta_weights(const size_t& batch_size, weights_delta_type& t) {
    static_assert(util::is_instantiation_of_v<DeltaWeightDims, weight_dims>, "DeltaWeightDims is an instantiation of weight_dims");
    t.resize(batch_size, DeltaWeightDims::out, DeltaWeightDims::in, DeltaWeightDims::h, DeltaWeightDims::w);
    t.setZero();
}

template <typename, typename = void>
struct has_weight_and_delta_helper : std::false_type {};

template <typename T>
struct has_weight_and_delta_helper<
    T,
    std::void_t<
        decltype(std::declval<T>().w),
        decltype(std::declval<T>().dw)
    >
> : std::true_type {};


template <typename, typename = void>
struct has_bias_and_delta_helper : std::false_type {};

template <typename T>
struct has_bias_and_delta_helper<
    T,
    std::void_t<
        decltype(std::enable_if_t<T::biased>()),
        decltype(std::declval<T>().b),
        decltype(std::declval<T>().db)
    >
> : std::true_type {};


template<typename T, typename EnableIf = void>
struct is_layer : std::false_type {};

template<typename T>
struct is_layer<T, std::void_t<decltype(std::declval<T>().is_trainable)>> : std::true_type {};


/**
 * \brief Check if a type is a layer
 */
template<typename Layer>
constexpr bool is_layer_v = is_layer<Layer>::value;

/**
 * \brief Check if a layer has delta weight states
 */
template<typename Layer>
constexpr bool has_weight_and_delta_v = has_weight_and_delta_helper<Layer>::value;

/**
 * \brief Check if a layer has delta bias states
 */
template<typename Layer>
constexpr bool has_bias_and_delta_v = has_bias_and_delta_helper<Layer>::value;

/**
 * \brief Apply weight initialization
 *
 * Attempts to apply weight initializer if available. Otherwise, all weights are
 * initialized simply to zero.
 */
template<typename Layer>
void apply_weight_initializer(Layer& layer) {
    if(layer.weight_initializer) {
        layer.weight_initializer(
            layer.w.data(),
            layer.w.data() + layer.w.size(),
            Layer::input_dims::size,
            Layer::output_dims::size
        );
    } else {
        layer.w.setZero();
    }
}

/**
 * \brief Apply either network or layer bias initializer
 */
template<typename Layer>
void apply_bias_initializer(Layer& layer) {
    if(layer.bias_initializer) {
        layer.bias_initializer(
            layer.b.data(),
            layer.b.data() + layer.b.size(),
            Layer::input_dims::size,
            Layer::output_dims::size
        );
    } else {
        layer.b.setZero();
    }
}

SP_ALGO_NN_DETAIL_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_DETAIL_LAYERS_HPP */

