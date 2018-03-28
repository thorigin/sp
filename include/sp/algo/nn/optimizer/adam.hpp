/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_OPTIMIZER_ADAM_HPP
#define SP_ALGO_NN_OPTIMIZER_ADAM_HPP

#include "optimizer.hpp"
#include <ratio>

enum policy {
    fixed,
    step,
} sdg_optimizer_policy;

template<
    sdg_optimizer_policy Policy = fixed,
    typename InitialRatio,
    size_t Step,
    typename Gamma
>
struct sdg_optimizer : optimizer<sdg_optimizer> {

};

#endif	/* SP_ALGO_NN_OPTIMIZER_ADAM_HPP */

