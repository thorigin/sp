/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MODEL_MITOSIS_STRATEGY_HPP
#define	SP_ALGO_GEN_MODEL_MITOSIS_STRATEGY_HPP

#include "pipeline.hpp"
#include "fitness_proportionate_unary_op.hpp"
#include "simulated_fission_op.hpp"
#include "stochastic_mutation_unary_op.hpp"
#include "uniform_mutation_rate.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Mitosis multiplication strategy
 */
template<std::intmax_t N_bit_ops = 2, std::intmax_t N = 1, std::intmax_t D = 100>
using mitosis_strategy =
    multiplication_pipeline_strategy<
    fitness_proportionate_unary_selection_op,
    simulated_binary_fission,
    stochastic_mutation_unary_op<
        uniform_mutation_rate<N, D>
    >
>;

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MODEL_MITOSIS_STRATEGY_HPP */

