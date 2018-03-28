/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_POOLING_OP_HPP
#define SP_ALGO_NN_LAYER_POOLING_OP_HPP

#include <cmath>
#include <limits>

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \file This file contains pooling ops
 */

/**
 * \brief Pool op base class.
 */
template<typename Derived>
struct pool_op {

    using derived_type = Derived;

    /**
     * Add sample to the pool operation
     */
    template<typename T>
    inline void sample(     const T& in,
                            const size_t& si,
                            const size_t& od,
                            const size_t& y,
                            const size_t& x) {
        derived().sample(in, si, od, y, x);
    }

    /**
     * Retrieve the result of the pool operation
     */
    inline auto result() {
        return derived().result();
    }

    derived_type& derived() const {
        return static_cast<derived_type&>(*this);
    }

};

/**
 * \brief Mean pooling operator
 */
struct mean_pooling_op : pool_op<mean_pooling_op> {

    mean_pooling_op() : total(0), samples(0) {}

    template<typename T>
    inline void sample(     const T& in,
                            const size_t& si,
                            const size_t& od,
                            const size_t& y,
                            const size_t& x) {
        ++samples;
        total += in;
    }

    inline auto result() {
        return total / samples;
    }

    float_t total;
    size_t samples;
};


/**
 * \brief Maximum pooling operator
 */
struct max_pooling_op : pool_op<max_pooling_op> {

    using indices_array = std::array<size_t, 4>;
    max_pooling_op() : max(std::numeric_limits<float_t>::min()), input_idx() {}

    template<typename T>
    inline void sample( const T& in,
                        const size_t& s,
                        const size_t& d,
                        const size_t& y,
                        const size_t& x) {
        using namespace std;
        if(in > max) {
            max = in;
            /* stores where the sample came from */
            input_idx = indices_array{s, d, y, x};
        }
    }

    inline auto result() {
        return max;
    }

    float_t max;

    /**
     * store the max indexes (s, d, y, x)
     */
    std::array<size_t, 4> input_idx;
};


SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_POOLING_OP_HPP */

