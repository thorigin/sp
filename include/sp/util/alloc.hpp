/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_UTIL_ALLOC_HPP
#define	SP_UTIL_ALLOC_HPP

#include <cstddef>
#include <cstdlib>
#include <type_traits>
#include "hints.hpp"
#include <boost/align/aligned_alloc.hpp>



namespace sp { namespace util {


/**
 * Allocation aligned memory
 *
 * \param alignment the alignment
 * \param size the number of elements to allocate
 * \return
 */
template<typename T, size_t alignment = 32>
sp_hot T aligned_alloc(const size_t& size) {
    static_assert(std::is_pointer<T>::value, "T must be a pointer type");
    return static_cast<T>(boost::alignment::aligned_alloc(alignment, size * sizeof(T)));
}

/**
 * Allocation aligned memory
 *
 * \param alignment the alignment
 * \param size the number of elements to allocate
 * \return
 */
template<typename T>
sp_hot T aligned_alloc(const size_t& alignment, const size_t& size) {
    static_assert(std::is_pointer<T>::value, "T must be a pointer type");
    return static_cast<T>(boost::alignment::aligned_alloc(alignment, size * sizeof(T)));
}

/**
 * Free an aligned memory
 *
 */
sp_hot void aligned_free(void* addr) {
    boost::alignment::aligned_free(addr);
}

}}

#endif /* SP_UTIL_ALLOC_HPP */

