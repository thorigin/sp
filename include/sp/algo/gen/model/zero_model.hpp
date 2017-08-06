/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MODEL_ZERO_MODEL_HPP
#define	SP_ALGO_GEN_MODEL_ZERO_MODEL_HPP

#include "model.hpp"
#include "../control/zero_fitness_evaluator.hpp"
#include "../control/zero_population_control.hpp"
#include "../migration/zero_model.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Zero Model
 *
 * Simply put a no-op model
 */
using zero_model = model<
        multiplication_pipeline_strategy<
            zero_selection_op,
            zero_recombination_op
        >,
        zero_population_control<zero_fitness_evaluator>,
        zero_migration_model
>;

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MODEL_ZERO_MODEL_HPP */

