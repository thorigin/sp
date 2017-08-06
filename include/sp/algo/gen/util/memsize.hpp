/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MEMSIZE_HPP
#define	SP_ALGO_GEN_MEMSIZE_HPP

#include <numeric>

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Mem Size of a container
 *
 * \param cont Must be a container of elements of dereferencable elements
 * having mem_size() function
 */
template<typename Container>
auto mem_size(Container& cont) {
    return sizeof(cont) + std::accumulate(
            cont.begin(),
            cont.end(),
            size_t(0),
            [](size_t& total, const auto& r) {
        return total + r->mem_size();
    });
}

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MEMSIZE_HPP */

