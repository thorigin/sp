/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_ACTIVATION_OP_HPP
#define SP_ALGO_NN_ACTIVATION_OP_HPP

#include <cmath>
#include "../../types.hpp"

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \file
 *
 * This file contains activation operation functors.
 * 
 * Contins a set of non-linear functions, or neuron activation function which
 * provide non-linearity
 */

/**
 * Base tag of activation op categories
 */
struct acitvation_op_category {};

/**
 * Applied as a unary function to all input values.
 * That is, for I_(x, y, ...) apply f (activation op) as
 * O_(x, y, ...) = f(I_(x, y, ...)), where the first x and y are the sample
 * index and input depth.
 */
struct activation_op_unary_category : acitvation_op_category {};

/**
 * Applied per depth slice.
 *
 * That is, for I_(x, y, z, ...) apply f (activation op) as
 * O_(x, y) = (I_(x, y, z, ...)), where the first x and y are the sample index
 * and input depth.
 */
struct activation_op_depth_slice_category : acitvation_op_category {};

/**
 * \brief Activator op base class.
 *
 *
 * Note: Derivations should take into account that the input value is x = op(y),
 *       therefore chain rule must be used.
 * 
 * \tparam Category the activation op
 */
template<typename Derived, typename Category = activation_op_unary_category>
struct activation_op {

    /**
     * The derivative of op(y) where y = op(x)
     */
    using derived_type = Derived;
    using category = Category;

    template<typename T>
    auto operator()(const T& in) const {
        return derived()(in);
    }

    valid_range range() const {
        return derived().range();
    }

    derived_type& derived() const {
        return static_cast<derived_type&>(*this);
    }

};

/**
 * \brief Identity activation op
 *
 * Performs identity activation (i.e. the same) value
 */
struct identity_activation_op : activation_op<identity_activation_op> {

    using derivative = identity_activation_op;

    template<typename T>
    auto& operator()(T& x) const {
        return x;
    }

    valid_range range() const {
        return {-1.0, 1.0};
    }
};

/**
 * \brief Sigmoid Derivative Activation Op
 */
struct sigmoid_deriv_activation_op;

/**
 * \brief Sigmoid Activation Op
 */
struct sigmoid_activation_op : activation_op<sigmoid_activation_op> {

    /**
     * The derivation of sigmoid_activation_op
     */
    using derivative = sigmoid_deriv_activation_op;

    template<typename T>
    auto operator()(const T& x) const {
        using namespace std;
        return 1.0 / (1.0 + exp(-x));
    }

    valid_range range() const {
        return {0.1, 0.9};
    }
};

/**
 * \brief Sigmoid Derivative Activation Op
 */
struct sigmoid_deriv_activation_op : activation_op<sigmoid_deriv_activation_op> {

    template<typename T>
    auto operator()(const T& x) const {
        return x * (1.0 - x);
    }

    valid_range range() const {
        return {0.1, 0.9};
    }

};

struct relu_deriv_activation_op;

struct relu_activation_op : activation_op<relu_activation_op> {

    /**
     * The derivative of sigmoid_activation_op
     */
    using derivative = relu_deriv_activation_op;

    template<typename T>
    auto operator()(const T& x) const {
        using namespace std;
        return max(x, 0);
    }

    valid_range range() const {
        return {0.1, 0.9};
    }
};

struct relu_deriv_activation_op : activation_op<relu_deriv_activation_op> {

    template<typename T>
    auto operator()(T&& x) const {
        return x < 0 ? 1 : 0;
    }

};

struct tanh_deriv_activation_op;

/**
 * \brief Tan Hyperbolic Activation Op
 */
struct tanh_activation_op : activation_op<tanh_activation_op> {

    /**
     * The derivation of tanh_activation_op
     */
    using derivative = tanh_deriv_activation_op;

    template<typename T>
    auto operator()(T&& x) const {
        using namespace std;
        return tanh(x);
    }

    valid_range range() const {
        return {-0.8, 0.8};
    }
};

/**
 * \brief Tan Hyperbolic Derivative Activation Op
 */
struct tanh_deriv_activation_op : activation_op<tanh_deriv_activation_op> {

    template<typename T>
    auto operator()(const T& x) const {
        using namespace std;
        //derivative (1 - tanh^2(y)), x = tanh(y)
        return (1.0f - x * x);
    }

    valid_range range() const {
        return {-0.8, 0.8};
    }
};


/**
 * \brief SoftMax Derivative Activation Op
 *
 * activation category : activation_op_depth_slice_category
 */
struct softmax_deriv_activation_op;

/**
 * \brief SoftMax Activation Op
 *
 * activation category : activation_op_depth_slice_category
 */
struct softmax_activation_op
        : activation_op<
            softmax_activation_op,
            activation_op_depth_slice_category
        > {

    /**
     * The derivation of tanh_activation_op
     */
    using derivative = softmax_deriv_activation_op;

    template<typename T>
    auto operator()(T&& x) const {
        throw "Not implemented";
    }

    valid_range range() const {
        return {0, 1.0};
    }
};

/**
 * \brief SoftMax Derivative Activation Op
 *
 * activation category : activation_op_depth_slice_category
 */
struct softmax_deriv_activation_op
    :   activation_op<
            softmax_deriv_activation_op,
            activation_op_depth_slice_category
        > {
    
    template<typename T>
    auto operator()(const T& x) const {
        throw "Not implemented";
    }

    valid_range range() const {
        return {0, 1.0};
    }
};

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_ACTIVATION_OP_HPP */

