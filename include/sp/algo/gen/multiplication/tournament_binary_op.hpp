/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATION_TOURNAMENT_BINARY_OP_HPP
#define	SP_ALGO_GEN_MULTIPLICATION_TOURNAMENT_BINARY_OP_HPP


SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Tournament Selection
 *
 *
 * This strategy considers the highest scoring fitness
 */
 struct tournament_selection_binary_op : binary_selection_strategy<tournament_selection_binary_op> {

    using operand_type = typename tournament_selection_binary_op::operand_type;
    using result_type = typename tournament_selection_binary_op::result_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type pair_id) {
        //Handle cases where pair_id has gone beyond half the population size,
        //wraps around as a solution
        const operand_type size = p->size();
        size_t first_idx = (pair_id*2 + 0) % size,
        second_idx = (pair_id*2 + 1) % size;
        dna* first = (*p)[first_idx];
        dna* second = (*p)[second_idx];
        return dna_ptr_pair(first, second);
    }

};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATION_TOURNAMENT_BINARY_OP_HPP */

