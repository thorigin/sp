/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MODEL_MEIOSIS_STRATEGY_HPP
#define	SP_ALGO_GEN_MODEL_MEIOSIS_STRATEGY_HPP

#include "pipeline.hpp"
#include "fitness_proportionate_binary_op.hpp"
#include "n_point_recombination_binary_op.hpp"
#include "stochastic_mutation_unary_op.hpp"
#include "uniform_mutation_rate.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Meiosis multiplication strategy
 *
 * Meiosis strategy replicates genetic material, which then divide into two more
 * daughter cells, producing 4 daughter cells with half the number of genetic
 * material of the parent cell
 *
 */
template<std::intmax_t N = 1, std::intmax_t D = 100>
using meiosis_strategy =
    multiplication_pipeline_strategy<
        fitness_proportionate_binary_selection_op,
        n_point_recombination_binary_op<1>,
        stochastic_mutation_unary_op<
            uniform_mutation_rate<N, D>
        >
    >;


SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MODEL_MEIOSIS_STRATEGY_HPP */

