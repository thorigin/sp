/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_CONTROL_ZERO_FITNESS_EVALUATOR_HPP
#define	SP_ALGO_GEN_CONTROL_ZERO_FITNESS_EVALUATOR_HPP

#include "sp/config.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Zero fitness Evaluator
 *
 * Simply returns a fitness score of 0 for every dna
 */
struct zero_fitness_evaluator {

    float operator()(const dna* const d) const {
        return 0.0f;
    }

};

SP_ALGO_GEN_NAMESPACE_END

#endif	/* SP_ALGO_GEN_CONTROL_ZERO_FITNESS_EVALUATOR_HPP */

