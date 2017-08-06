/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATIONN_BIT_BASED_MUTATION_UNARY_OP_HPP
#define	SP_ALGO_GEN_MULTIPLICATIONN_BIT_BASED_MUTATION_UNARY_OP_HPP

#include "pipeline_op.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/***
 * \brief n bit mutation strategy based on probability for each bit
 * chromosome per DNA.
 *
 * \todo optimize, slow, N * 4 rand() function calls needed
 */
template<std::intmax_t N>
struct n_bit_based_mutation_binary_op
    :  mutation_op<
            n_bit_based_mutation_binary_op<
                N
            >
        > {

    static_assert(N >= 1, "N must be an greater or equal to 1");

    using result_type = typename n_bit_based_mutation_binary_op::result_type;

    using operand_type = typename n_bit_based_mutation_binary_op::operand_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type d) {
        for(std::intmax_t i = 0; i < N; ++i) {
            size_t ch_idx = d->size() * m->random().rand_float();
            chromosome* ch = (*d)[ch_idx];
            size_t  bits_size = ch->size(),
                    bit_idx = bits_size * m->random().rand_float();
            ch->flip(bit_idx);
        }
        return d;
    }
};

using single_bit_mutation_op = n_bit_based_mutation_binary_op<1>;

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATIONN_BIT_BASED_MUTATION_UNARY_OP_HPP */

