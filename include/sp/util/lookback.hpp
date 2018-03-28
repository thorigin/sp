/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_UTIL_LOOKBACK_HPP
#define SP_UTIL_LOOKBACK_HPP

#include "sp/config.hpp"
#include "../util/hints.hpp"
#include <vector>


SP_UTIL_NAMESPACE_BEGIN

/**
 * \brief Provides historical n point based lookup for statistical functions
 * such as mean, min, max, derivative, etc
 *
 * Provides a fixed numeric of look back values
 */
template<typename T>
struct fixed_lookback {

    using value_type = T;
    using key_type = size_t;

    fixed_lookback(key_type lookbacks)
        : index(0), lookups() {
        lookups.resize(lookbacks);
    }

    void tick(value_type&& value) { 
        lookups[index++ % lookups.size()] = std::move(value);
    }

    value_type mean(const key_type& lookback)  {
        value_type mean = 0;
        for_every(lookback, [&](const value_type& val) { mean += val; });
        return mean / lookback;
    }

    value_type min(key_type lookback) {
        value_type min = std::numeric_limits<value_type>::max();
        for_every(lookback, [&](const value_type& val) { if(min > val) min = val; });
        return min;
    }

    value_type max(key_type lookback) {
        value_type max = 0;
        for_every(lookback, [&](const value_type& val) { if(max < val) max = val; });
        return max;
    }

    /**
     * Derivative
     */
    value_type deriv(key_type lookback) {
        value_type delta_sum = 0;
        value_type last = 0;
        bool first = true;
        for_every(lookback, [&](const value_type& val) {
            if(!first) {
                delta_sum += val - last;
                last = val;
            } else {                
                last = val;
                first = false;
            }
        });
        return delta_sum / (lookback-1);
    }

    template<typename T2>
    friend std::ostream& operator<<(std::ostream& os, const fixed_lookback<T2>& lb) {
        os << "History(" << lb.lookups.size() << ", " << lb.index << ") { ";
        for(size_t i = 0, len = lb.lookups.size(); i < len; ++i) {
            if(i != 0) {
                os << ',';
            }
            os << lb.lookups[i];
        }
        os << "}";
        return os;
    }

protected:

    template<typename Action>
    sp_hot void for_every(key_type lookback, Action&& action) {
        int size = static_cast<int>(lookups.size());       
        for(    int i = static_cast<int>(index) - static_cast<int>(lookback),
                len = static_cast<int>(index);
                i < len;
                ++i) {
            size_t idx = (i % size + size) % size;
            action(lookups[idx]);
        }
    }

private:
    key_type index;
    std::vector<T> lookups;
};

/**
 * \brief Provides historical n point based lookup for statistical functions
 * such as mean, min, max, derivative, etc
 *
 * Provides a rolling numeric of look back values
 * Drawback is that it is unable to calculate the actual results accurately
 */
template<typename T>
struct rolling_lookback {

    using value_type = T;
    using key_type = size_t;
    using weight_type = float;

    rolling_lookback(weight_type update_weight) :
        weight(update_weight), min_val(), max_val(), mean_val(),
        deriv_val(), last(), min_dist(), max_dist() {
        BOOST_ASSERT_MSG(update_weight >0.0f && update_weight < 1.0f, "Weight must be in the range (0, 1)");
    }

    void tick(value_type&& value) {
        value_type new_mean = mean_val * (1.0f - weight) + value * weight;
        mean_val = new_mean;
        deriv_val = (value - last) * (1.0f - weight) + (value - last) * weight;
        last = value;
    }

    value_type mean()  {
        return mean_val;
    }

    value_type min() {
        return min_val;
    }

    value_type max() {
        return max_val;
    }

    /**
     * Derivative
     */
    value_type deriv() {
        return deriv_val;
    }

    template<typename T2>
    friend std::ostream& operator<<(std::ostream& os, const rolling_lookback<T2>& lb) {

        return os;
    }

private:
    weight_type weight;
    value_type min_val;
    value_type max_val;
    value_type mean_val;
    value_type deriv_val;
    value_type last;
    size_t min_dist;
    size_t max_dist;
};


SP_UTIL_NAMESPACE_END

#endif /* SP_UTIL_LOOKBACK_HPP */