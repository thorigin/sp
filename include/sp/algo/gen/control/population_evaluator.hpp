/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_CONTROL_POPULATION_EVALUATOR_HPP
#define	SP_ALGO_GEN_CONTROL_POPULATION_EVALUATOR_HPP

#include <algorithm>
#include "sp/config.hpp"
#include "../domain/population.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Population Evaluator
 *
 * Responsible assigning a fitness score to every individual chromosome in the
 * population using the specified Fitness evaluator to evaluate the fitness
 * score of each chromosome in the population.
 * \param Fitness_evaluator_type the fitnesss evaluator responsible for
 */
template<typename FitnessEvaluator, typename Derived>
struct population_evaluator {

    using fitness_evaluator_type = FitnessEvaluator ;

    using derived_type = Derived ;

    template<typename Model>
    sp_hot
    void operator()(Model* m, population* pop, size_t from, size_t to) {
        derived().impl(m, pop, from, to);
    }

    /**
     * CRTP helper
     */
    derived_type& derived() {
        return *static_cast<derived_type*>(this);
    }

};


SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_CONTROL_POPULATION_EVALUATOR_HPP */

