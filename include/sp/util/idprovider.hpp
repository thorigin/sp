/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_UTIL_IDPROVIDER_HPP
#define	SP_UTIL_IDPROVIDER_HPP

#include <memory>
#include <atomic>
#include <random>
#include <atomic>

namespace sp { namespace util {

/**
 * \brief Helper class to provide thread safe unique integer based identity
 * sequences
 *
 * \param starting_id
 */
template<typename T = uint64_t>
struct unique_id_provider {

    using value_type = T;

    static_assert(std::is_integral<T>::value, "Type T must be an integer type");

    unique_id_provider(T starting_id) : counter(starting_id) {}

    T next_id() { return ++counter; }

private:
    std::atomic<T> counter;

};


}}

#endif /* SP_UTIL_IDPROVIDER_HPP */

