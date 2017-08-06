/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATIONFITNESS_PROPORTIONATE_BINARY_OP_HPP
#define	SP_ALGO_GEN_MULTIPLICATIONFITNESS_PROPORTIONATE_BINARY_OP_HPP

#include "pipeline_op.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN


/**
 * \brief Fitness Proportionate Binary Selection
 *
 * linear O(n))
 *
 * Also known as Roulette Wheel Selection
 */
struct fitness_proportionate_binary_selection_op
    : binary_selection_strategy<fitness_proportionate_binary_selection_op> {

    using operand_type = typename fitness_proportionate_binary_selection_op::operand_type;
    using result_type = typename fitness_proportionate_binary_selection_op::result_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type pair_id) {

        dna* first = nullptr;
        dna* second = nullptr;

        auto sum_fitness = std::accumulate(p->begin(), p->end(), float(0), [](const float& sum, const dna* d) { return sum + d->eval(); });
        float value = m->random().rand_float() * sum_fitness;

        //select the first
        for(auto it = p->begin(), end = p->end(); it != end; ++it) {
            auto d = *it;;
            value -= d->eval();
            if(value <= 0) {
                first = d;
                break;
            }
        }
        //select the second
        float value2 = m->random().rand_float() * sum_fitness;

        for(auto it = p->begin(), end = p->end(); it != end; ++it) {
            auto d = *it;
            value2 -= d->eval();
            //if value is less and not the first value
            if(value2 <= 0 && d != first) {
                second = d;
                break;
            }
        }

        if(!first) {
           first = (*p)[p->size()-2];
        }
        if(!second) {
            second = (*p)[p->size()-1];
        }

        return dna_ptr_pair(first, second);
    }

};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATIONFITNESS_PROPORTIONATE_BINARY_OP_HPP */

