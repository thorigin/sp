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
#include "../types.hpp"
#include "sp/util/hints.hpp"


SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \brief Mean Square Error Derivative
 */
struct mean_square_error_derivative {

    sp_hot void operator()(         const size_t& si,
                                    const tensor_4& predicted,
                                    const tensor_4& observed,
                                    tensor_4& result) {

        BOOST_ASSERT(predicted.dimensions() == observed.dimensions());

        const size_t m = predicted.dimension(1) * predicted.dimension(2) * predicted.dimension(3);

        float_t factor = float_t(2.0f) / static_cast<float_t> (m);

        result.chip(si, 0) = factor * (predicted.chip(si, 0) - observed.chip(si, 0));
    }
};

/**
 * \brief Mean Square Error
 */
struct mean_square_error {

    using derivative_type = mean_square_error_derivative;

    auto operator()(                    const size_t& si,
                                        const tensor_4& predicted,
                                        const tensor_4& observed) {
        BOOST_ASSERT(predicted.dimensions() == observed.dimensions());

        tensor_0 d = (
            (predicted.chip(si, 0) - observed.chip(si, 0))
                *
            (predicted.chip(si, 0) - observed.chip(si, 0))
        ).sum();

        return d(0) / static_cast<float_t> (predicted.size());
    }

    derivative_type derivative;
};


SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LOSS_MEAN_SQUARE_ERROR_HPP */

