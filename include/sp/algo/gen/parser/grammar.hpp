/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_PARSER_HPP
#define	SP_ALGO_PARSER_HPP

#include "sp/config.hpp"
#include "../gen.hpp"
#include <vector>
#include <tuple>
#include <bitset>
#include <type_traits>
#include <variant>
#include <optional>

SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN

/**
 * \brief Grammar Genetic Expression Generator
 *
 * Provides for the generation of actionable results
 * based on the specified grammar
 */
template<
    typename Grammar,
    typename ... Arguments,
    typename Reader = chromosome_bit_to_integral_reader<>
>
struct grammar_genetic_expression_generator
    : genetic_expression_generator<
            typename Grammar::result_type,
            grammar_genetic_expression_generator<Grammar>,
            Reader
        > {

    using grammar_type = Grammar;
    using reader_type = Reader;

    grammar_genetic_expression_generator(grammar_type&& grammar)
        : grammar(std::forward<grammar_type>(grammar)) {}

    typename grammar_type::result_type (const dna* const ch) const {
        
    }

     grammar_type grammar;
};

SP_ALGO_GEN_PARSER_NAMESPACE_END


#endif	/* SP_ALGO_PARSER_HPP */

