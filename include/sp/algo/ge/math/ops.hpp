/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_GE_MATH_OPS_HPP
#define	SP_ALGO_GEN_GE_MATH_OPS_HPP

namespace sp { namespace algo { namespace gen { namespace math_ops {

template<typename T>
using plus = std::plus<T>;

template<typename T>
using minus = std::minus<T>;

template<typename T>
using divides = std::divides<T>;

template<typename T>
using multiplies = std::multiplies<T>;

template<typename T>
struct pow : std::binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return std::pow(x, y);
    }
};

template<typename T>
struct exp : std::unary_function<T, T> {
    T operator()(const T& x) const {
        return std::exp(x);
    }
};

template<typename T>
struct cos : std::unary_function<T, T> {
    T operator()(const T& x) const {
        return std::cos(x);
    }
};

template<typename T>
struct sin : std::unary_function<T, T> {
    T operator()(const T& x) const {
        return std::sin(x);
    }
};

template<typename T>
struct neg : std::unary_function<T, T> {
    T operator()(const T& x) const {
        return -x;
    }
};

}}}}


#endif	/* SP_ALGO_GEN_GE_MATH_OPS_HPP */

