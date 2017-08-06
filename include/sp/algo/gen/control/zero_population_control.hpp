/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_CONTROL_ZERO_POPULATION_CONTROL_HPP
#define	SP_ALGO_GEN_CONTROL_ZERO_POPULATION_CONTROL_HPP

#include "sp/config.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Zero population control
 */
template<
    typename Fitness_evaluator_type,
    typename Population_evaluator_type = default_population_evaluator<Fitness_evaluator_type>,
    bool Prune_before_multiplication = true
>
struct zero_population_control
    : population_control<
            zero_population_control<
                Fitness_evaluator_type,
                Population_evaluator_type,
                Prune_before_multiplication
            >,
            Fitness_evaluator_type,
            Population_evaluator_type,
            Prune_before_multiplication
        > {

    template<typename Model>
    sp_hot size_t add_count_impl(Model*, population* p) {
        return 0;
    }

    template<typename Model>
    sp_hot size_t prune_count_impl(Model* m, population* p) {
        return 0;
    }
};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_CONTROL_ZERO_POPULATION_CONTROL_HPP */

