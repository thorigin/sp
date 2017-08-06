/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MIGRATION_MODEL_HPP
#define	SP_ALGO_GEN_MIGRATION_MODEL_HPP

#include "sp/config.hpp"
#include "../domain/population.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Migration Model manages the migration between the different populations
 * and islands
 */
template<typename Derived>
struct migration_model {

    using derived_type = Derived;

    template<typename Model>
    void operator()(Model* m, population* p) {
        return derived().impl(m, p);
    }

    /**
     * CRTP helper
     */
    derived_type & derived() {
        return *reinterpret_cast<derived_type*>(this);
    }

};


SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MIGRATION_MODEL_HPP */

