/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_CONTROL_POPULATION_CONTROL_HPP
#define	SP_ALGO_GEN_CONTROL_POPULATION_CONTROL_HPP

#include "sp/config.hpp"
#include "./../domain/population.hpp"
#include "default_population_evaluator.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \param Derived_type
 * \param Fitness_evaluator_type
 * \param Population_evaluator_type
 * \param Prune_before_multiplication Whether or not DNAs are removed from the pool
 * before or after multiplication. Note: Doing it before can be more memory efficient.
 */
template<
    typename Derived,
    typename FitnessEvaluator,
    typename Population_evaluator_type = default_population_evaluator<FitnessEvaluator>,
    bool Prune_before_multiplication = true
>
struct population_control {

    using derived_type = Derived;
    using fitness_evaluator_type = FitnessEvaluator;
    using population_evaluator_type = Population_evaluator_type;

    constexpr static bool prune_before_multiplication = Prune_before_multiplication;

    template<typename Model>
    sp_hot void evaluate(Model* m, population* p, size_t from, size_t to) {
        derived().evaluate_impl(m, p, from, to);
    }

    template<typename Model>
    sp_hot size_t add_count(Model* m, population* p) {
        return derived().add_count_impl(m, p);
    }

    template<typename Model>
    sp_hot size_t prune_count(Model* m, population* p) {
        return derived().prune_count_impl(m, p);
    }

    /**
     * CRTP helper
     */
    derived_type& derived() {
        return *static_cast<derived_type*>(this);
    }
};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_CONTROL_POPULATION_CONTROL_HPP */

