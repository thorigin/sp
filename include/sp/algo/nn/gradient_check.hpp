/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_TESTING_GRADIENT_CHECK_HPP
#define	SP_ALGO_TESTING_GRADIENT_CHECK_HPP

#include <cmath>
#include <limits>
#include <random>
#include <vector>
#include <iterator>
#include <utility>
#include <memory>

#include <boost/assert.hpp>

#include "sp/config.hpp"
#include "random.hpp"
#include "sp/algo/nn/matrix.hpp"
#include "sp/algo/nn/types.hpp"
#include "sp/algo/nn/layer/detail/layers.hpp"
#include "layer/layer.hpp"


/**
 * @file Gradient check and validations
 *
 *
 * References:
 * - http://ufldl.stanford.edu/wiki/index.php/Gradient_checking_and_advanced_optimization
 * - http://cs231n.github.io/neural-networks-3/#gradcheck
 */

SP_ALGO_NN_NAMESPACE_BEGIN

namespace detail {

    /**
     * Helper
     * @param res
     */
    template<typename Tensor>
    void generate_uniform_into(Tensor& res) {
        auto& gen = random_generator::get();
        std:: uniform_real_distribution<float_t> dist(-1.0f, 1.0);
        std::for_each(res.data(), res.data() + res.size(), [&](auto& v) {
            v = dist(gen);
        });
    }

    template<typename T>
    void zero_range(T* start, T* end) {
        std::for_each(start, end, [](auto& v) { v = 0.0f; });
    }
}

/**
 * \brief Generate a random input_type
 */
template<typename Layer>
tensor_4 generate_inputs_for(const Layer& layer, const size_t& batch_size = 1) {
    using dims = typename Layer::input_dims;
    tensor_4 res(batch_size, dims::d, dims::h, dims::w);
    detail::generate_uniform_into(res);
    return res;
}

namespace detail {

    template<typename Container, typename TensorOrLayer>
    void layer_raw_data_vector_helper(Container& cont, TensorOrLayer& tol) {
        if constexpr (detail::is_layer_v<TensorOrLayer>) {
            if constexpr(detail::has_weight_and_delta_v<TensorOrLayer>) {
                BOOST_ASSERT(tol.w.size() > 0);
                cont.emplace_back(tol.w.data(), tol.w.data() + tol.w.size());
            }
            if constexpr(detail::has_bias_and_delta_v<TensorOrLayer>) {
                BOOST_ASSERT(tol.b.size() > 0);
                cont.emplace_back(tol.b.data(), tol.b.data() + tol.b.size());
            }
        } else {
            BOOST_ASSERT(tol.size() > 0);
            cont.emplace_back(tol.data(), tol.data() + tol.size());
        }
    }
}

/**
 * \brief A vector of pairs containing pointers to the range which the element
 *        of a layer, such as input, weight, bias are defined for.
 */
using layer_raw_data_vector_type = std::vector<std::pair<float_t*, float_t*>>;

/**
 * \brief Helper function that converts a layer and its data to a vector of
 *        pairwise range of pointers representing each tensor of different ranks
 *        such that they can be accessed index-wise regardless of dimensionality.
 */
template<typename ... TensorOrLayers>
layer_raw_data_vector_type
layer_raw_data_vector(TensorOrLayers& ... tols) {
   layer_raw_data_vector_type data;
   data.reserve(1 + sizeof...(TensorOrLayers));
   (detail::layer_raw_data_vector_helper(data, tols),  ...);
   return data;
}

/**
 * \brief Calculate the numerical gradient
 *
 *
 * Procedure is outlined in https://github.com/amaas/stanford_dl_ex/blob/master/ex1/grad_check.m
 * \param get_perturb_ref Returns a reference to the location to perturb.
 *        Function is invoked as get_perturb_ref(layer, in, out).
 * \param get_result Retrieves the output of the f(x). Same arguments as before.
 */
template<typename Layer>
float_t numerical_gradient(         Layer                               layer,
                                    tensor_4                          in,
                                    const std::pair<size_t, size_t>&    in_indexes,
                                    const size_t&                       out_index) {
    /**
     * sqrt(epsilon)
     * https://scicomp.stackexchange.com/questions/14355/choosing-epsilons
     */
    constexpr float_t h = std::sqrt(std::numeric_limits<float_t>::epsilon());
    const size_t batch_size = in.dimension(0);

    tensor_4 out;
    detail::prepare_and_zero_tensor<typename Layer::output_dims>(batch_size, out);

    /* retrieve array of pairs of pointers to floats, */
    auto in_data = layer_raw_data_vector(in);
    auto out_data = layer_raw_data_vector(out);

    auto& preturb = in_data[in_indexes.first].first[in_indexes.second];
    auto prev = preturb;

    preturb = prev - h;
    layer.forward_prop(in, out);
    float_t fx1 = out_data[0].first[out_index];

    //reset
    out.setZero();

    preturb = prev + h;
    layer.forward_prop(in, out);
    float_t fx2 = out_data[0].first[out_index];

    return (fx2 - fx1) / ( float_t(2.0f) * h);
}

/**
 * \brief Calculate the analytical gradient
 *
 * \param get_result Retrieves the output of the f'(x). Invoked as
 *        '''get_result(layer, in_delta,
 */
template<typename Layer>
float_t analytical_gradient(    Layer                               layer,
                                tensor_4                          prev_out,
                                const std::pair<size_t, size_t>&    in_indexes,
                                const size_t&                       out_index) {
    const size_t batch_size = prev_out.dimension(0);

    tensor_4 prev_delta;
    tensor_4 curr_out;
    tensor_4 curr_delta;

    /* initialize parameters */
    detail::prepare_and_zero_tensor<typename Layer::input_dims>(batch_size, prev_delta);
    detail::prepare_and_zero_tensor<typename Layer::output_dims>(batch_size, curr_out);
    detail::prepare_and_zero_tensor<typename Layer::output_dims>(batch_size, curr_delta);

    /* retrieve array of pairs of pointers to floats */
    auto in_grad= layer_raw_data_vector(prev_delta);
    auto out_grad = layer_raw_data_vector(curr_delta);

    out_grad[0].first[out_index] = 1.0f;

    layer.forward_prop(prev_out, curr_out);

    layer.backward_prop(prev_out, prev_delta, curr_out, curr_delta);

    return in_grad[in_indexes.first].first[in_indexes.second];
}



/**
 * \brief Calculates the relative error given the numerical gradient and the
 *        analytical gradient.
 */
float_t relative_error(float_t numerical_gradient, float_t analytical_gradient) {
    return (
            std::abs(numerical_gradient - analytical_gradient)
            /
            (std::max(std::abs(numerical_gradient), std::abs(analytical_gradient)))
    );
}



/**
 * \brief Selects a random element n out of Layer::output_dims::size
 */
template<typename Layer>
size_t gradient_random_output(const Layer& layer) {
    auto& rand = random_generator::get();
    return rand() % Layer::output_dims::size;
}

/**
 * \brief Selects a random input tensor and a random element n
 *        out of that
 */
template<typename Layer>
std::pair<size_t, size_t> gradient_random_input(const Layer& layer) {
    auto& rand = random_generator::get();
    size_t in_data_size = 1;
//    if constexpr(detail::has_weight_and_delta_v<Layer>) { ++in_data_size; }
    //    if constexpr(detail::has_bias_and_delta_v<Layer>)   { ++in_data_size; }
    auto data_idx = rand() % in_data_size;
    auto element_idx = rand() % Layer::input_dims::size;
    return {data_idx, element_idx};
}

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_TESTING_GRADIENT_CHECK_HPP */

