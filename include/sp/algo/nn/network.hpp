/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_NETWORK_HPP
#define SP_ALGO_NN_NETWORK_HPP

#include <tuple>
#include <array>
#include <fstream>
#include <iosfwd>
#include <experimental/filesystem>

#include "sp/util/timing.hpp"

#include "sp/util/for_each.hpp"
#include "sp/util/tuples.hpp"
#include "sp/util/hints.hpp"
#include "sp/util/typename.hpp"

#include "sp/config.hpp"
#include "matrix.hpp"
#include "loss.hpp"
#include "optimizer.hpp"
#include "types.hpp"
#include "normalize.hpp"
#include "weight.hpp"


SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \file Definition of network composite structure
 */

/**
 * \brief Test result (alias, tuple)
 */
using test_results = std::tuple<
    size_t,
    size_t,
    std::unordered_map<
        size_t,
        std::unordered_map<
            size_t,
            size_t
        >
    >
>;

/**
 * \brief Generic Neural Network composite structure
 */
template<typename ... Layers>
struct network {

    using layers_type = std::tuple<Layers...>;
    constexpr static size_t layers_count = std::tuple_size_v<layers_type>;

    using input_layer_type = typename std::tuple_element_t<0, layers_type>;
    using output_layer_type = typename std::tuple_element_t<layers_count-1, layers_type>;
    using input_dims  = typename std::tuple_element_t<0, layers_type>::input_dims;
    using output_dims = typename std::tuple_element_t<layers_count-1, layers_type>::output_dims;

    network() : layers() {
        weight_initializer = glorot_weight_initializer();
        bias_initializer = fixed_weight_initializer(0);
    }

    /**
     * \brief Configure and prepare network for the given batch size and
     *        optionally reset the weights. Must be called before use. It is
     *        undefined behavior not to configure network before use.
     */
    sp_hot void configure(const size_t& batch_size, bool reset = false) {
        size_t idx = 0;
        if(batch_size != batch_size_config || reset) {
            util::for_each(layers, [&](auto& layer) {
                using layer_type = std::decay_t<decltype(layer)>;

                /**
                 * Check if layer has weight or biases, then propagate
                 * default network initializers to those, if not explicitly configured
                 */
                if constexpr(detail::has_weight_and_delta_v<layer_type>) {
                    if(!layer.weight_initializer) {
                        layer.weight_initializer = weight_initializer;
                    };
                }
                if constexpr(detail::has_bias_and_delta_v<layer_type>) {
                    if(!layer.bias_initializer) {
                        layer.bias_initializer = bias_initializer;
                    }
                }

                /* prepare input and input deltas */
                detail::prepare_tensor<typename layer_type::input_dims >(batch_size, values[idx]);
                detail::prepare_tensor<typename layer_type::input_dims>(batch_size, values_delta[idx]);

                layer.configure(batch_size, reset);

                ++idx;
            });
        }
        batch_size_config = batch_size;
        /* output of network */
        detail::prepare_tensor<output_dims>  (batch_size, values[layers_count]);
        detail::prepare_tensor<output_dims>  (batch_size, values_delta[layers_count]);
    }

    sp_hot tensor_4& forward(tensor_4& input) {
        values[0] = input;
        size_t idx = 0;
            BOOST_ASSERT_MSG(
            detail::validate_dimensions<input_dims>(batch_size(), input),
            "Input dimensions match configuration"
        );
        util::for_each(layers, [&](auto& layer) {
            /* clear output */
            values[idx+1].setZero();
            layer.forward_prop(values[idx], values[idx+1]);
            ++idx;
        });
        return values[layers_count];
    }

    sp_hot void backward(tensor_4& delta) {
        BOOST_ASSERT_MSG(
            detail::validate_dimensions<output_dims>(batch_size(), delta),
            "Delta dimensions match configuration"
        );
        size_t idx = layers_count;
        /* Set the current delta of the output layer */
        values_delta[layers_count] = delta;
        util::for_each(util::reverse(layers), [&](auto& layer) {
            values_delta[idx-1].setZero();
            layer.backward_prop(
                values[idx-1], /* previous layer input */
                values_delta[idx-1], /* previous layer delta */
                values[idx], /* current output */
                values_delta[idx] /* current delta */
            );
            --idx;
        });
    }

    /**
     * \brief Update weights of network
     */
    template<typename Optimizer>
    sp_hot void update_weights(Optimizer& optimizer) {
        //util::scoped_timer t("update_weights(..)");
        util::for_each(layers, [&](auto& layer) {
            layer.update_weights(optimizer);
        });
    }

    /**
     * \brief Perform forward propagation and return index
     * @return the maximum index vector of samples
     */
    std::vector<size_t> forward_max_index(tensor_4& input) {
        BOOST_ASSERT(input.dimension(0) == 1); // only supported for now
        const size_t batch_size = input.dimension(0);
        tensor_4& out = forward(input);
        std::vector<size_t> res(batch_size);
        res[0] = std::distance(
            out.data(),
            std::max_element(out.data(), out.data()+out.size())
        );
        return res;
    }

    /**
     * \brief Perform testing on the neural network by performing  forward
     *        propagation on samples and compare with the labels using
     *        maximum index. Assumes inputs are normalized.
     */
    auto test(sample_vector_type& samples, class_vector_type& classes) {
        BOOST_ASSERT(samples.size() > 0);
        BOOST_ASSERT(classes.size() > 0);
        BOOST_ASSERT(samples.size() == classes.size());
        test_results result;


        const size_t sample_count = samples.size();

        const size_t prev_batch_size = batch_size();

        /**
         * Reconfigure for testing, resizes the layer buffers
         * \todo Test in batches
         */
        configure(1);

        for(size_t s = 0; s < sample_count; ++s) {
            values[0].chip(0, 0) = samples[s]; //omit copying
            auto predicted = this->forward_max_index(values[0])[0];
            auto actual = classes[s];
            if(predicted == actual) {
                std::get<0>(result) += 1;
            }
            std::get<2>(result)[predicted][actual] += 1;
        }
        std::get<1>(result)= sample_count;

        /* restore previous configuration */
        configure(prev_batch_size);

        return result;
    }

    /**
     * Load the network weights from the input stream
     */
    bool load(std::istream& is) {
        this->configure(1, true);
        util::for_each(layers, [&](auto& layer) {
            layer.load(is);
        });
        return !!is;
    }

    /**
     * \brief Load the weights from the file specified
     */
    bool load(const std::string& file) {
        std::fstream fs(file);
        return load(fs);
    }

    /**
     * \brief Save the network weights to the output stream
     * @param os
     */
    bool save(std::ostream& os) {
        auto flags = os.flags();
        os << std::setprecision(12);
        util::for_each(layers, [&](auto& layer) {
            layer.save(os);
        });
        os.flags(flags);
        return !!os;
    }

    bool save(const std::string& file) {
        std::ofstream fs(file, std::ios::out);
        return save(fs);
    }

    auto& out_layer() {
        return std::get<layers_count-1>(layers);
    }

    auto& in_layer() {
        return std::get<0>(layers);
    }

    /**
     * \brief The output range of the network
     */
    valid_range out_range() {
        return out_layer().range();
    }

    /**
     * The input range of the network
     */
    valid_range in_range() {
        return in_layer().range();
    }

    /**
     * The target output range of the network, for training purposes
     * @return
     */
    valid_range out_target_range() {
        return out_layer().target_range();
    }

    template<size_t Index>
    std::tuple_element_t<Index, std::tuple<Layers...>>& get() {
        return std::get<Index>(layers);
    }

    size_t batch_size() {
        return batch_size_config;
    }

    //Tuple holds all the layers of this network
    std::tuple<Layers...> layers;

    /**
     * Store values and values delta of inputs and ouputs
     * + 1 (input layer)
     */
    std::array<tensor_4, layers_count + 1> values;
    std::array<tensor_4, layers_count + 1> values_delta;

    /**
     * \brief Provides a plug-in point for custom weight initialization strategy
     *
     * Strategy is passed on to layers if weight and bias initializers have not
     * been manually initialized
     */
    std::function<void(float_t*, float_t*, const size_t&, const size_t&)> weight_initializer;
    std::function<void(float_t*, float_t*, const size_t&, const size_t&)> bias_initializer;

protected:
    size_t batch_size_config;
};

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_NETWORK_HPP */

