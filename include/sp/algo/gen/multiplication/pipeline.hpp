/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATION_PIPELINE_HPP
#define	SP_ALGO_GEN_MULTIPLICATION_PIPELINE_HPP

#include "sp/config.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Multiplication strategy
 *
 * Encapsulates the various details of multiplication of the model
 *
 * For complete implementations, see mitosis_stratety, and meiosis_strategy
 *
 * Multiplication strategies take the model as an argument, the population to
 * perform multiplication within, and a multi_count parameter specying how many
 * multiplications to perform. Results are added to the population on
 * completion.
 *
 * Adds the new resulting chromosomes at the end of the population
 */
template<typename ... Operators>
struct multiplication_pipeline_strategy {

    template<typename Model>
    sp_hot void operator()(Model* m, population* p, const size_t& multi_count) {
        repeat<Model, Operators...>(m, p, multi_count);
    }

private:

    static_assert(sizeof...(Operators) >= 2, "Multiplication Pipeline strategy should consist of at least one selection and one recombination operations");

    template<typename Model, typename First, typename ... Others>
    sp_hot void repeat(Model* m, population* p, const size_t& multi_count) {
        for(size_t i = 0; i < multi_count; ++i) {
            auto res = pipe<Model, Others...>(m, p, multi_count, First()(m, p, multi_count, i));
            p->add(res);
        }
    }

    template<typename Model, typename First, typename Second, typename ... Others>
    sp_hot typename First::result_type pipe(
        Model* m,
        population* p,
        const size_t& multi_count,
        typename First::operand_type input) {
        return pipe<Model, Second, Others...>(m, p, multi_count, First()(m, p, multi_count, input));
    }

    //Base case
    template<typename Model, typename Last>
    sp_hot typename Last::result_type pipe(
        Model* m,
        population* p,
        const size_t& multi_count,
        typename Last::operand_type input) {
        return Last()(m, p, multi_count, input);
    }

};


SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATION_PIPELINE_HPP */

