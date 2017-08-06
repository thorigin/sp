/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATIONSTOCHASTIC_MUTATION_UNARY_OP_HPP
#define	SP_ALGO_GEN_MULTIPLICATIONSTOCHASTIC_MUTATION_UNARY_OP_HPP

#include "pipeline_op.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/***
 * \brief Stochastic Mutation Strategy
 *
 * Considers all bits equal based on a Mutation_rate_type specified
 *
 * \todo optimize. It is slow because 1 rand() func call is needed
 * per bit per chromosome per DNA
 */
template<typename MutationRate>
struct stochastic_mutation_unary_op
    :  mutation_op<
            stochastic_mutation_unary_op<
                MutationRate
            >
        > {

    using result_type = typename stochastic_mutation_unary_op::result_type;

    using operand_type = typename stochastic_mutation_unary_op::operand_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type d) {
        size_t cl = d->size();
        for(size_t ci = 0; ci < cl; ++ci) {
            chromosome* ch = (*d)[ci];
            for(size_t i = 0, l = ch->size(); i < l; ++i) {
                if(MutationRate()(m, d) >= m->random().rand_float()) {
                    size_t bit_idx = ch->size() * m->random().rand_float();
                    ch->flip(bit_idx);
                }
            }
        }
        return d;
    }
};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATIONSTOCHASTIC_MUTATION_UNARY_OP_HPP */

