/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_UTIL_RAND_HPP
#define	SP_UTIL_RAND_HPP

#include <random>
#include <memory>
#include <limits>
#include <set>
#include <boost/random.hpp>

#include "sp/config.hpp"
#include "hints.hpp"

SP_UTIL_NAMESPACE_BEGIN

/**
 * \brief Random utility
 *
 * Encapsulates common usage cases for uniform real distributions of float, double type,
 * and any integral type.
 */
template<typename Random_engine_type = boost::random::mt19937_64> //mt19937, consider default_random_engine
struct rand_util {

    using random_engine_type = Random_engine_type;
    using result_type = typename random_engine_type::result_type;

public:

    rand_util(uint seed = 0) : rng(), distFl(0, 1), distDl(0,1) {
        if (seed > 0) {
            rng.seed(seed);
        } else {
            rng.seed(std::random_device{}());
        }
    }

    sp_hot float rand_float() {
        return distFl(rng);
    }

    sp_hot double rand_double() {
        return distDl(rng);
    }

    /**
     * Generate a random integral in the range of [from, to)
     * \param from
     * \param to
     * \return
     */
    template<typename T>
    sp_hot T rand_integral(T from, T to) {
        if(std::numeric_limits<T>::digits10 > std::numeric_limits<float>::digits10) {
            return (to - from) * distDl(rng) + from;
        } else {
            return (to - from) * distFl(rng) + from;
        }
    }
        /**
     * Creates a completely randomized integer from
     * \return
     */
    sp_hot typename random_engine_type::result_type rand_integral() {
        return this->rng();
    }

    /**
     * Return the random engine being used
     */
    sp_hot random_engine_type& eng() {
        return this->rng;
    }

private:
    random_engine_type rng;
    boost::random::uniform_real_distribution<float> distFl;
    boost::random::uniform_real_distribution<double> distDl;
};

typedef std::shared_ptr<rand_util<>> rand_util_ptr;

inline rand_util_ptr new_rand() {
    return std::make_shared<rand_util<>>();
}


/**
 * Generated N unique numbers in the range [from, to)
 *
 * \param n
 * \param from
 * \param to
 * \return set of n unique numbers
 */
template<size_t N, typename result_type, typename Rand_util_type>
sp_hot std::array<result_type, N> n_unique(Rand_util_type& util, result_type from, result_type to) {
    std::set<result_type> s;
    //select n points
    for(result_type i = 0; i < N; ++i) {
        result_type rand_idx;
        do {
            rand_idx = util.rand_integral(from, to);
        } while(s.find(rand_idx) != s.end());
        s.insert(rand_idx);
    }
    std::array<result_type, N> arr;
    std::copy(s.begin(), s.end(), arr.begin());
    return arr;
}

SP_UTIL_NAMESPACE_END


#endif /* SP_UTIL_RAND_HPP */

