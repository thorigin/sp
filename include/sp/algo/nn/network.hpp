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
#include <experimental/filesystem>


#include "sp/util/for_each.hpp"
#include "sp/util/tuples.hpp"
#include "sp/util/hints.hpp"

#include "sp/config.hpp"
#include "matrix.hpp"
#include "loss.hpp"
#include "optimizer.hpp"
#include "types.hpp"
#include "normalize.hpp"


SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \file Definitions of network composite structure and common typedefs
 */

/**
 * Test result 
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
 * \brief Training
 */
template<
    size_t BatchSize = 16,
    size_t Epochs = 1,
    typename Optimizer = ada_gradient_optimizer<>,
    typename LossFunction = mean_square_error
>
struct training {

    /**
     * Optimizer type
     */
    using optimizer_type = Optimizer;

    /**
     * Loss function type
     */
    using loss_function_type = LossFunction;

    /**
     * Batch size
     */
    constexpr static size_t batch_size = BatchSize;

    static_assert(BatchSize > 0, "BatchSize is greater or equal to 1");

    /**
     * Number of training epochs
     */
    constexpr static size_t epochs = Epochs;

    template<typename Network>
    sp_hot void operator()(Network& network, sample_vector_type& samples, class_vector_type& classes) {

        BOOST_ASSERT_MSG(!samples.empty(), "Inputs is not empty");
        BOOST_ASSERT_MSG(samples.size() == classes.size(), "Inputs and classes size mismatch");

        const size_t input_count = samples.size();
        const size_t batch_count = input_count / batch_size;

        std::vector<input_type>  inputs           = normalize(network, batch_size, samples);
        std::vector<output_type> expected_outputs = normalize(network, batch_size, classes);        
        
        for(size_t i = 0; i < epochs; ++i) {
            for(size_t b = 0; b < batch_count; ++b) {
                (*this)(network, inputs[b], expected_outputs[b]);
            }
        }
    }

    /**
     * \brief Perform a single training operation
     */
    template<typename Network>
    sp_hot void operator()(Network& network, input_type& input, output_type& expected_output) {
        output_type& actual = network.forward(input);
        auto grad = gradient(loss, expected_output, actual);
        network.backward(grad);
        network.template update_weights(optimizer);
    }

    optimizer_type optimizer;
    loss_function_type loss;
};

/**
 * \brief Calculate the the gradient of the expected vs actual output
 */
template<typename LossFunction>
auto gradient(LossFunction& loss, output_type& expected, output_type& actual) {
    //BOOST_ASSERT(expected.dimensions() == actual.dimensions());
    return loss.derivative(expected, actual);
}

/**
 * \brief Generic Neural Network composite structure
 */
template<typename ... Layers>
struct network {

    using layers_type = std::tuple<Layers...>;
    constexpr static size_t layers_count = std::tuple_size_v<layers_type>;

    using input_dims  = typename std::tuple_element_t<0, layers_type>::input_dims;
    using output_dims = typename std::tuple_element_t<layers_count-1, layers_type>::output_dims;

    network() : layers() {}

    //static_assert() //assert layer series dimensions
//
//    sp_hot void prepare(const size_t& samples) {
//        util::for_each(layers, [&](auto& layer) {
//
//        }
//    }

    sp_hot output_type& forward(input_type& input) {
        values[0] = input;
        /* layers start at 1, 0 is the input */
        size_t idx = 0;
        util::for_each(layers, [&](auto& layer) {
            /**
             * Perform forward propatation of layer,
             * given the inputs and outputs of current layer
             */
            layer.forward_prop(values[idx], values[idx+1]);
            ++idx;
        });
        return values[idx];
    }

    sp_hot void backward(current_delta_type& delta) {
        size_t idx = layers_count;

        /**
         * Set the current delta of the output layer
         */
        values_delta[layers_count] = delta;

        util::for_each(util::reverse(layers), [&](auto& layer) {
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
        util::for_each(layers, [&](auto& layer) {
            layer.update_weights(optimizer);
        });
    }

    /**
     * \brief Perform forward propagation and return index
     * @param input
     * @return
     */
    size_t forward_max_index(input_type& input) {
        //auto res = input.argmax();
        auto res = forward(input);
        //auto argmax = res.argmax();
        throw "Not implemented";
        return 0;
    }

    /**
     * \brief Perform testing on the neural network by performing  forward
     *        propagation on samples and compare with the labels using
     *        maximum index
     * @param samples
     * @param labels
     */
    template<typename SamplesContainer, typename LabelsContainer>
    auto test(SamplesContainer& samples, LabelsContainer& labels) {        
        test_results result;
        const size_t sample_count = samples.size();
        for(size_t s = 0; s < sample_count; ++s) {
            auto predicted = this->forward_max_index(samples[s]);
            auto actual = labels[s];
            if(predicted == actual) {
                std::get<0>(result) += 1;
            }
            std::get<1>(result) += 1;
            std::get<2>(result)[predicted][actual] += 1;

        }
        return result;
    }

    void load(const std::string& file) {        
        std::fstream fs(file);        
        util::for_each(layers, [&](auto& layer) {
            //layer.load(fs);
        });
    }

    void save(const std::string& file) {
        std::fstream fs(file);
        util::for_each(layers, [&](auto& layer) {
            //layer.save(fs);
        });
    }

    /**
     * \brief Provides a range of the network output, as a tuple of [min,max]
     * 
     * @return a tuple of min and max
     */
    valid_range out_range() {
        return std::get<layers_count-1>(layers).range();
    }

    /** \TODO possibly remove, unused */
    valid_range in_range() {
        return std::get<0>(layers).range();
    }

    //Tuple holds all the layers of this network
    std::tuple<Layers...> layers;

    /**
     * Store values and values delta of inputs and ouputs
     * + 1 (input layer)
     */
    std::array<input_type, layers_count + 1> values;
    std::array<input_type, layers_count + 1> values_delta;

};
    
SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_NETWORK_HPP */

