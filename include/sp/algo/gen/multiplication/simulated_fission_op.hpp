/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATIONSIMULATED_FISSION_OP_HPP
#define	SP_ALGO_GEN_MULTIPLICATIONSIMULATED_FISSION_OP_HPP

#include "pipeline_op.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Simulated Binary Fission
 *
 *
 * Note: The name binary fission may be confusing, as it returns
 * only a single DNA, whereas the original simply remains in the pool
 */
struct simulated_binary_fission
    : multiplication_pipeline_op<
            dna*,
            dna*,
            simulated_binary_fission
        > {

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type d) {
        return new dna(*d);
    }
};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATIONSIMULATED_FISSION_OP_HPP */

