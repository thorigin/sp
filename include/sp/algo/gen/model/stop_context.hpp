/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_STOP_CONTEXT_HPP
#define	SP_ALGO_GEN_STOP_CONTEXT_HPP

#include "sp/config.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Evolution stop context
 *
 * Works on a per population basis
 *
 */
struct stop_context {

    stop_context(const population* const pop) : p(pop) {}

    float highest_eval() const  {
        float highest = -1;
        size_t len = to;
        for(size_t i = from; i < len; ++i) {
            float current = (*p)[i]->eval();
            if(current > highest)
                highest = current;
        }
        return highest;
    }

    size_t evol_count() const {
        return p->evolutions();
    }

    float average_fit() const  {
        float total = std::accumulate(
            p->begin() + from,
            p->begin() + to,
            float(0),
            [](float total, const dna* const d) {
                return total + d->eval();
            }
        );
        return (total / (to-from));
    }

    /**
     * Sets the range of DNAs to consider for evaluation context
     *
     *
     * @param from
     * @param to
     */
    void set_valid_range(size_t from, size_t to) {
        this->from = from;
        this->to = to;
    }
private:
    const population* const p;
    size_t from, to;
};


/**
 * \brief Macro helper that makes a boolean expression with a context bound variable
 * ctx of type stop_context available for shorthand expressions,
 * simplifies lambda generations into easily readable condition.
 *
 * For example:
 * Given a model variable m, and population variable pop
 * then the evolve call simplifies to:
 *
 *      m.evolve(pop, sp_evolve_until(ctx.evol_count() >= 1000))
 *
 * \param expression boolean expression
 * \return result of expression
 */
#define sp_evolve_until(expression) [](const stop_context& ctx) { return (expression); }

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_STOP_CONTEXT_HPP */

