/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#include "sp/config.hpp"

#ifndef SP_ALGO_
#ifndef SP_ALGO_GE_GE_HPP

namespace sp { namespace algo { namespace gen {

/**
 * \brief Genetic Expression Generator super type
 *
 * The base type of all possible genetic expressions. It takes the address
 * of a Chromosome and returns the produced expression. The process of which is implementation
 * defined
 * 
 * The result can be of any type. However, see genetic_expression for actionable
 * results
 */
template<
    typename Result,
    typename Derived,
    typename Reader
>
struct genetic_expression_generator {

    using result_type = Result;
    using derived_type = Derived;
    using reader_type = Reader;

    result_type operator()(const chromosome* const ch) const {
        return derived().impl(ch);
    }

    derived_type& derived() {
        return *static_cast<derived_type*>(this);
    }
};

}}}

#endif	/* SP_ALGO_GE_GE_HPP */

