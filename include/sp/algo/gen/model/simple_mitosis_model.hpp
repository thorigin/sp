/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MODEL_SIMPLE_MITOSIS_MODEL_HPP
#define	SP_ALGO_GEN_MODEL_SIMPLE_MITOSIS_MODEL_HPP

#include "model.hpp"
#include "../multiplication/mitosis_strategy.hpp"
#include "../control/ratio_population_control.hpp"
#include "../migration/zero_model.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Simple Mitosis based Evolutionary Programming Model
 */
template<typename FitnessEvaluator>
using simple_mitosis_model = model<
    mitosis_strategy<>,
    ratio_population_control<
        std::ratio<0>,
        std::ratio<25,100>,
        FitnessEvaluator
    >,
    zero_migration_model
>;

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MODEL_SIMPLE_MITOSIS_MODEL_HPP */

