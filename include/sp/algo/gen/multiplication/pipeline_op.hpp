/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATION_OP_HPP
#define	SP_ALGO_GEN_MULTIPLICATION_OP_HPP

#include "sp/config.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Multiplication Pipeline Operation
 *
 */
template<typename Operand, typename Result, typename Derived_type>
struct multiplication_pipeline_op {

    using operand_type = Operand;
    using result_type = Result;
    using derived_type = Derived_type;


    template<typename Model>
    result_type operator()(Model* m, population* p, const size_t& multi_count, operand_type operand) {
        return derived().impl(m, p, multi_count, operand);
    }

    /**
     * CRTP helper
     */
    derived_type & derived() {
        return *static_cast<derived_type*>(this);
    }
};

template<typename Operand>
struct noop_unary_pipeline_op
    : multiplication_pipeline_op<Operand, Operand, noop_unary_pipeline_op<Operand>> {

    using operand_type = Operand;
    using result_type = Operand;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type operand) {
        return operand;
    }
};

/**
 * \brief Selection Strategy
 *
 * The method in which to select the DNAs to be considered for propagation
 */
template<typename Result, typename Derived_type>
struct selection_op
    : multiplication_pipeline_op<
            const size_t&, //pair id
            Result,
            Derived_type
        > {
    using operand_type = const size_t&;
    using result_type = Result;
};

template<typename Derived>
struct unary_selection_op
    : selection_op<
            dna*,
            Derived
        > {

    using operand_type = typename unary_selection_op::operand_type;

    using result_type = typename unary_selection_op::result_type;
};

template<typename Derived>
struct binary_selection_strategy
    : selection_op<
            dna_ptr_pair,
            Derived
        > {

    using operand_type = typename binary_selection_strategy::operand_type;

    using result_type = typename binary_selection_strategy::result_type;

};

struct zero_selection_op : unary_selection_op<zero_selection_op> {

    using operand_type = typename zero_selection_op::operand_type;
    using result_type = typename zero_selection_op::result_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type id) {
        return nullptr;
    }
};

/**
 * \brief Recombination Operator
 *
 * Possible aspects of which this operator covers:
 * * Crossover
 * * Gene Conversion
 * * Nonhomologous recombination
 *
 * Takes two or more chromosomes and combines them into an entirely new
 * resulting genetic combination depending on the implementation
 *
 */
template<typename Operand, typename Derived>
struct recombination_op
    : multiplication_pipeline_op<Operand, dna*, Derived> {
    using operand_type = typename recombination_op::operand_type;
    using result_type = typename recombination_op::result_type;
};


/**
 * \brief Zero Recombination Op
 *
 * No-op
 */
struct zero_recombination_op
    : recombination_op<dna*, zero_recombination_op> {

    using operand_type = typename recombination_op<dna*, zero_recombination_op>::operand_type;
    using result_type = typename recombination_op<dna*, zero_recombination_op>::result_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type pair) {
        return nullptr;
    }
};


/***
 * Mutation strategy
 */
template<typename Derived>
struct mutation_op : multiplication_pipeline_op<dna*, dna*, Derived> {

    using operand_type = typename mutation_op::result_type;
    using result_type = typename mutation_op::result_type;

};

template<typename Derived>
struct mutation_rate {

    using derived_type = Derived;

    template<typename Model>
    float operator()(Model* m, dna* d) {
        return derived().impl(m, d);
    }

    /**
     * CRTP helper
     */
    derived_type const& derived() const {
        return *static_cast<derived_type const*>(this);
    }

};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATION_OP_HPP */

