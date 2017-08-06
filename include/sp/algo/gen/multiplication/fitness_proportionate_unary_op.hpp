/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATIONFITNESS_PROPORTIONATE_UNARY_OP_HPP
#define	SP_ALGO_GEN_MULTIPLICATIONFITNESS_PROPORTIONATE_UNARY_OP_HPP

#include "pipeline_op.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN


/**
 * \brief Fitness Proportionate Unary Selection
 *
 * linear O(n))
 *
 * Also known as Roulette Wheel Selection
 */
struct fitness_proportionate_unary_selection_op : unary_selection_op<fitness_proportionate_unary_selection_op> {

    using operand_type = typename fitness_proportionate_unary_selection_op::operand_type;
    using result_type = typename fitness_proportionate_unary_selection_op::result_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type pair_id) {
        dna* selection = nullptr;

        auto sum_fitness = std::accumulate(p->begin(), p->end(), float(0), [](const float& sum, const dna* d) { return sum + d->eval(); });
        float value = m->random().rand_float() * sum_fitness;

        for(auto it = p->begin(), end = p->end(); it != end; ++it) {
            value -= (*it)->eval();
            if(value <= 0) {
                selection = *it;
                break;
            }
        }

        if(!selection) {
           selection = (*p)[p->size()-1];
        }

        return selection;
    }

};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATIONFITNESS_PROPORTIONATE_UNARY_OP_HPP */

