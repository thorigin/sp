/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATION_TOURNAMENT_UNARY_OP_HPP
#define	SP_ALGO_GEN_MULTIPLICATION_TOURNAMENT_UNARY_OP_HPP


SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Tournament Selection Strategy
 *
 */
 struct tournament_selection_unary_op
    : unary_selection_op<tournament_selection_unary_op> {

    using operand_type = typename tournament_selection_unary_op::operand_type;
    using result_type = typename tournament_selection_unary_op::result_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type pair_id) {
        return (*p)[pair_id];
    }

};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATION_TOURNAMENT_UNARY_OP_HPP */

