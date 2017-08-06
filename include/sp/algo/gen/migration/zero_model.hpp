/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MIGRATION_ZERO_MODEL_HPP
#define	SP_ALGO_GEN_MIGRATION_ZERO_MODEL_HPP

#include "model.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Zero Migration Model
 */
struct zero_migration_model : migration_model<zero_migration_model> {

    template<typename Model>
    void impl(Model* m, population* p) {}

};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MIGRATION_ZERO_MODEL_HPP */

