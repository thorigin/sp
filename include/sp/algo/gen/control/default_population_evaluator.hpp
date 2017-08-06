/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_CONTROL_DEFAULT_POPULATION_EVALUATOR_HPP
#define	SP_ALGO_GEN_CONTROL_DEFAULT_POPULATION_EVALUATOR_HPP

#include <algorithm>
#include "sp/config.hpp"
#include "population_evaluator.hpp"
#include "../domain/population.hpp"


SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Population Evaluator
 */
template<typename FitnessEvaluator>
struct default_population_evaluator
    : population_evaluator<
            FitnessEvaluator,
            default_population_evaluator<FitnessEvaluator>
        > {

    using fitness_evaluator_type = FitnessEvaluator;

    template<typename Model>
    sp_hot void impl(Model* m, population* pop, size_t from, size_t to) {
        FitnessEvaluator evaluator;
        for(size_t i = from; i < to; ++i) {
            dna* c = (*pop)[i];
            c->set_eval(evaluator(c));
        }
        std::stable_sort(
            pop->begin(),
            pop->end(),
            highest_dna_eval_comparator()
        );
    }

};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_CONTROL_DEFAULT_POPULATION_EVALUATOR_HPP */

