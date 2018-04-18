/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */


#ifndef SP_ALGO_NN_WEIGHT_WEIGHT_HPP
#define SP_ALGO_NN_WEIGHT_WEIGHT_HPP

#include <random>
#include <functional>
#include <algorithm>
#include <iterator>
#include <cmath>

#include "../config.hpp"
#include "../types.hpp"
#include "../random.hpp"


/**
 * \file Weight initialization strategies
 */

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Abstract Weight Initializer
 */
template<typename Derived>
struct weight_initializer {

    using derived_type = Derived;

    void operator()(float_t* begin, float_t* end, const size_t& fan_in, const size_t& fan_out) {
        BOOST_ASSERT(begin <= end);
        derived().impl(begin, end, fan_in, fan_out);
    }

    /**
     * CRTP helper
     */
    derived_type& derived() {
        return *static_cast<derived_type*>(this);
    }

};

/**
 * \brief Lecun Weight Initializer
 */
struct lecun_weight_initializer : weight_initializer<lecun_weight_initializer> {

    void impl(float_t* start, float_t* end, const size_t& fan_in, const size_t& fan_out) {
        const float_t scale = 1.0f / std::sqrt(fan_in);
        auto& gen = random_generator::get();
        std::uniform_real_distribution<float_t> dist(-scale, scale);
        std::for_each(start, end, [&](float_t& val) {
            val = dist(gen);
        });
    }
};


/**
 * \brief Glorot Weight Initializer
 */
struct glorot_weight_initializer : weight_initializer<glorot_weight_initializer>  {

    void impl(float_t* start, float_t* end, const size_t& fan_in, const size_t& fan_out) {
        const float_t r = std::sqrt( 6.0f / (fan_in + fan_out));
        auto& gen = random_generator::get();
        std::uniform_real_distribution<float_t> dist(-r, r);
        std::for_each(start, end, [&](float_t& val) {
            val = dist(gen);
        });
    }
};

/**
 * \brief Gaussian weight initializer
 *
 * Fills the weight tensor with a gaussian distribution of the range [a, b)
 */
struct gauss_weight_initializer : weight_initializer<gauss_weight_initializer>  {

    gauss_weight_initializer(float_t a_ = -1.0f, float_t b_ = 1.0f) : a(a_), b(b_) {}

    void impl(float_t* start, float_t* end, const size_t& fan_in, const size_t& fan_out) {
        auto& gen = random_generator::get();
        std::uniform_real_distribution<float_t> dist(a, b);
        std::for_each(start, end, [&](float_t& val) {
            val = dist(gen);
        });
    }

    float_t a;
    float_t b;
};

/**
 * \brief Fixed weight initializer. Initializer weights to a fixed value.
 */
struct fixed_weight_initializer : weight_initializer<fixed_weight_initializer>  {

    fixed_weight_initializer(float_t val = 0.0f) : value(val) {}

    void impl(float_t* start, float_t* end, const size_t& fan_in, const size_t& fan_out) {
        std::for_each(start, end, [&](float_t& val) {
            val = value;
        });
    }

    float_t value;
};

/**
 * \brief Vector weight initializer. Initializes weights to a vector of values.
 */
struct vector_weight_initializer : weight_initializer<vector_weight_initializer>  {

    vector_weight_initializer(std::vector<float_t> val) : values(val) {}

    void impl(float_t* start, float_t* end, const size_t& fan_in, const size_t& fan_out) {
    BOOST_ASSERT_MSG(static_cast<size_t>(std::distance(start, end)) == values.size(), "Input dimensions must match vector list");
        std::copy(values.begin(), values.end(), start);
    }

    std::vector<float_t> values;
};

SP_ALGO_NN_NAMESPACE_END

#endif /* SP_ALGO_NN_WEIGHT_WEIGHT_HPP */
