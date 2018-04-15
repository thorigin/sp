/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_PARAMS_HPP
#define SP_ALGO_NN_LAYER_PARAMS_HPP

/**
 * \file Contains definitions for parameters for the different layer kinds
 */

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Volume Dimensions used to descrie both input and output volumes
 *        dimensionality statically, at compile time.
 */
template<
    size_t Depth = 1,
    size_t Height = 1,
    size_t Width = 1
>
struct volume_dims {

    /**
     * Validate inputs
     */
    static_assert(Depth > 0,     "Depth must be greater or equal to 1");
    static_assert(Height > 0,    "Height must be greater or equal to 1");
    static_assert(Width > 0,     "Width must be greater or equal to 1");

    /**
     * Artificial limit to reduce user errors (including overflows)
     */
    static_assert(Depth <= 1e15 && Height <= 1e15 && Width <= 1e15, "Depth, Height, and Width must not exceed 10x15");

    /**
     * Depth
     */
    constexpr static size_t d = Depth;

    /**
     * Height
     */
    constexpr static size_t h = Height;

    /**
     * Width
     */
    constexpr static size_t w = Width;

    /**
     * The total size of the volume
     */
    constexpr static size_t size = d * w * h;

    /**
     * The total area of the volume (width by height)
     */
    constexpr static size_t area = w * h;

    /**
     *
     */
    template<typename VolumeDims>
    constexpr static bool isEqual(VolumeDims vol = VolumeDims()) {
        return  d == VolumeDims::d &&
                h == VolumeDims::h &&
                w == VolumeDims::w;
    }
};

/**
 * Weight dimensions template parameter struct
 */
template<
    size_t Outputs = 1,
    size_t Inputs = 1,
    size_t Height = 1,
    size_t Width = 1
>
struct weight_dims {

    /**
     * Validate parameters
     */
    static_assert(Outputs > 0,   "Outputs must be greater or equal to 1");
    static_assert(Inputs > 0,    "Inputs must be greater or equal to 1");
    static_assert(Height > 0,    "Height must be greater or equal to 1");
    static_assert(Width > 0,     "Width must be greater or equal to 1");

    /**
     * Outputs
     */
    constexpr static size_t out = Outputs;

    /**
     * Outputs
     */
    constexpr static size_t in = Inputs;

    /**
     * Height
     */
    constexpr static size_t h = Height;

    /**
     * Width
     */
    constexpr static size_t w = Width;
};

/**
 * \brief padding
 */
enum padding_type {
    /**
     * \brief pad_w = pad_h = 0. Also known as zero padding.
     */
    valid,
    /**
     * pad_h = R/2 and pad_w = S / 2`
     */
    same,
};


/**
 * \brief Kernel parameters
 */
template<
    size_t Depth = 64,
    size_t Height = 2,
    size_t Width = 2,
    size_t StrideWidth = 2,
    size_t StrideHeight = 2,
    padding_type PaddingType = padding_type::valid
>
struct kernel_params {

    static_assert(Depth > 0,            "Depth (Filter count) must be greater or equal to one");
    static_assert(Width > 0,            "Width must be greater or equal to one");
    static_assert(Height > 0,           "Height must be greater or equal to one");
    static_assert(StrideWidth > 0,     "StrideWidth must be greater than zero");
    static_assert(StrideHeight > 0,    "StrideHeight must be greater than zero");

    constexpr static size_t d = Depth;
    constexpr static size_t h = Height;
    constexpr static size_t w = Width;
    constexpr static size_t s_w = StrideWidth;
    constexpr static size_t s_h = StrideHeight;
    constexpr static padding_type padding = PaddingType;

};

/**
 * \brief Kernel parameters with symmetrical dimensionality
 *
 * @tparma Depth number of output channels, i.e. number of kernels
 * @tparam Length (Width and Height) value
 * @tparam Stride (Width and Height) value
 * @tparam Padding padding type
 */
template<
    size_t Depth = 64,
    size_t Length = 2,
    size_t Stride = 2,
    padding_type Padding = padding_type::valid
>
using kernel_symmetric_params = kernel_params<Depth, Length, Length, Stride, Stride, Padding>;

/**
 * \brief Pooling kernel parameters with symmetrical dimensionality
 *
 * @tparam Length (Width and Height) value
 * @tparam Stride (Width and Height) value (Defaults to Length)
 * @tparam Padding padding type
 */
template<
    size_t Length = 2,
    size_t Stride = Length,
    padding_type Padding = padding_type::valid
>
using pooling_kernel_params = kernel_params<1, Length, Length, Stride, Stride, Padding>;

using kernel_params_default = kernel_symmetric_params<>;


SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_PARAMS_HPP */

