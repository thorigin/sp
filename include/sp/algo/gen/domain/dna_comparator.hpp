/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_DNA_COMPARATOR_HPP
#define	SP_ALGO_GEN_DNA_COMPARATOR_HPP

#include "sp/util/hints.hpp"
#include "dna.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief DNA comparator
 *
 * Compares the evaluation (eval) of each DNA
 * Where a higher eval value has higher precedence
 */
struct highest_dna_eval_comparator {

    sp_hot bool operator()(const dna* const l, const dna* const r) const {
        return l->eval() > r->eval();
    }
};

/**
 * \brief DNA comparator
 *
 * Compares the evaluation (eval) of each DNA
 * Where a lower eval value has lower precedence
 */
struct lowest_dna_eval_comparator {

    sp_hot bool operator()(const dna* const l, const dna* const r) const {
        return l->eval() < r->eval();
    }
};


SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_DNA_COMPARATOR_HPP */

