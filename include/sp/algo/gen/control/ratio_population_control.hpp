/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_CONTROL_RATIO_POPULATION_CONTROL_HPP
#define	SP_ALGO_GEN_CONTROL_RATIO_POPULATION_CONTROL_HPP

#include <ratio>
#include <cmath>
#include "sp/config.hpp"
#include "population_control.hpp"
#include "default_population_evaluator.hpp"
#include "../domain/population.hpp"


SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Population Growth by a ratio (percentage)
 *
 * Note: Growth and replace ratios are greedy, i.e. always round up. This is in
 * part due to low population size yielding no change if ratio is very low
 * \tparam GrowthRatio
 * \tparam ReplaceRatio
 * \tparam FitnessEvaluator the fitness evaluator type
 * \tparam PopulationEvaluator
 * \tparam PruneBeforeMultiplication
 * \tparam RoundResultsUp (optional) default true
 */
template<
    typename GrowthRatio,
    typename ReplaceRatio,
    typename FitnessEvaluator,
    typename PopulationEvaluator = default_population_evaluator<FitnessEvaluator>,
    bool PruneBeforeMultiplication = true,
    bool RoundResultsUp = true
>
struct ratio_population_control
    : population_control<
            ratio_population_control<
                GrowthRatio,
                ReplaceRatio,
                FitnessEvaluator,
                PopulationEvaluator,
                PruneBeforeMultiplication
            >,
            FitnessEvaluator,
            PopulationEvaluator,
            PruneBeforeMultiplication
        > {

    using growth_ratio = GrowthRatio;
    using replace_ratio = ReplaceRatio;

    static constexpr bool round_result_up = RoundResultsUp;

    static constexpr float growth_perc = static_cast<float>(growth_ratio::num) / static_cast<float>(growth_ratio::den);
    static_assert(growth_perc >= 0.0f, "Growth_ratio must be greater or equal to zero");
    static constexpr float replace_perc = static_cast<float>(replace_ratio::num) / static_cast<float>(replace_ratio::den);
    static_assert(replace_perc >= 0.0f, "Replace_ratio must be greater or equal to zero");

    ratio_population_control() : evaluator() {}

    template<typename Model>
    sp_hot void evaluate_impl(Model* m, population* p, size_t from, size_t to) {
        evaluator(m, p, from, to);
    }

    template<typename Model>
    sp_hot size_t add_count_impl(Model*, population* p) {
        if(round_result_up) {
            return std::ceil(p->size() * (replace_perc + growth_perc));
        } else {
            return p->size() * (replace_perc + growth_perc);
        }
    }

    template<typename Model>
    sp_hot size_t prune_count_impl(Model* m, population* p) {
        if(round_result_up) {
            return std::ceil(p->size() * replace_perc);
        } else {
            return p->size() * replace_perc;
        }
    }

    PopulationEvaluator evaluator;
};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_CONTROL_RATIO_POPULATION_CONTROL_HPP */

