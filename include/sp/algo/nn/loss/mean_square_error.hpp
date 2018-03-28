/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */
#ifndef SP_ALGO_NN_LOSS_MEAN_SQUARE_ERROR_HPP
#define SP_ALGO_NN_LOSS_MEAN_SQUARE_ERROR_HPP

#include <boost/assert.hpp>
#include "../config.hpp"
#include "../matrix.hpp"


SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Mean Square Error Derivative
 */
struct mean_square_error_derivative {
    
    output_type operator()(const output_type &y, const output_type &t) {
        BOOST_ASSERT(y.dimensions() == t.dimensions());
        BOOST_ASSERT(y.size() == t.size());
        
        output_type d(y.dimensions());
        const size_t len = t.size();

        float_t factor = float_t(2) / static_cast<float_t> (len);

        for(size_t i = 1, len = y.size(); i < len; ++i) {
            d.data()[i] += factor * (y.data()[i] - t.data()[i]);
        }

        return d;
    }
};

/**
 * \brief Mean Square Error
 */
struct mean_square_error {

    using derivative_type = mean_square_error_derivative;
    
    float_t operator()(const output_type &y, const output_type& t) {
        BOOST_ASSERT(y.size() == t.size());

        float_t d = 0;
        const size_t len = y.size();
        for(size_t i = 1; i < len; ++i) {
            d += (y.data()[i] - t.data()[i]) * (y.data()[i] - t.data()[i]);
        }

        return d / static_cast<float_t> (y.size());
    }

    derivative_type derivative;
};


SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LOSS_MEAN_SQUARE_ERROR_HPP */

