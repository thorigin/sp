/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_CONFIG_HPP
#define SP_ALGO_NN_CONFIG_HPP

#include <cmath>

#include "sp/config.hpp"

SP_ALGO_NN_NAMESPACE_BEGIN

#ifndef NN_FLOAT_TYPE
#define NN_FLOAT_TYPE std::float_t
#endif

#ifndef SP_ALGO_NN_RANDOM_GENERATOR
#define SP_ALGO_NN_RANDOM_GENERATOR std::mt19937
#endif

using float_t = NN_FLOAT_TYPE;
        
SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_CONFIG_HPP */

