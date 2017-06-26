/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_STATS_HPP
#define	SP_ALGO_STATS_HPP

#include <iosfwd>
#include <iomanip>
#include <utility>
#include <vector>
#include <string>
#include <cmath>
#include <numeric>
#include <limits>
#include <chrono>

/**
 * Statistics
 */
namespace sp { namespace algo {

template<typename Fp_type = double>
struct stat_results;

template<typename Fp_type = double>
std::ostream& operator<<(std::ostream& os, const stat_results<Fp_type>& res);

template<typename Fp_type>
struct stat_results {

    using floating_point_type = Fp_type;
    using entry = std::pair<std::string, floating_point_type>;

    template<typename Fp_type_>
    friend std::ostream& operator<<(std::ostream& os, const stat_results<Fp_type_>& res);

    stat_results(std::string&& title)
        :   title(std::forward<std::string>(title)),
            created(std::chrono::system_clock::now()),
            entries() {}

    stat_results(const std::string& title)
        :   title(title),
            created(std::chrono::system_clock::now()),
            entries() {}

    std::string title;
    std::chrono::system_clock::time_point created;
    std::vector<entry> entries;
};

template<typename Fp_type>
std::ostream& operator<<(std::ostream& os, const stat_results<Fp_type>& res) {
    os  << "--- BEGIN: " << res.title << " - "
        << res.created.time_since_epoch().count() << '\n';
    auto flags = os.flags();
    os << std::fixed << std::setprecision(7);
    for(auto& e : res.entries) {
        os <<  std::setw(24) << e.first << ":" <<  std::setw(16) << e.second << '\n';
    }
    os << "--- END\n";
    os.setf(flags);
    return os;
}

/**
 * Creates a statistics result object
 *
 * Passed in cont must be sorted (asc or desc) by the UnaryOp extr
 * \param cont the input container
 * \param extr the extractor
 * \return the stat_result object
 */
template<typename Container, typename UnaryOp, typename Fp_type = double>
const stat_results<Fp_type> make_stats( std::string&& title,
                                        const Container& cont,
                                        UnaryOp extr) {
    using fp_type = Fp_type;
    stat_results<fp_type> r(std::forward<std::string>(title));

    auto begin = std::begin(cont), end = std::end(cont);
    size_t dist = std::distance(begin, end);

    fp_type total = std::accumulate(begin, end, fp_type(0),
            [&](const fp_type& total, const typename Container::value_type val) { return total + extr(val); }
    );

    fp_type max = nanf("");
    fp_type min = nanf("");

    fp_type mean = total / dist;
    fp_type median = dist % 2 == 0 ? (( extr(cont[dist/2-1]) + extr(cont[dist/2]))/2) : extr(cont[dist/2]);
    fp_type std_dev = std::sqrt(
        std::accumulate(std::begin(cont), std::end(cont), fp_type(0),
            [&](const fp_type& total, const typename Container::value_type val) {
                fp_type val_i = extr(val);
                return total + (val_i - mean) * (val_i - mean);
            }
        )
        / dist
    );

    fp_type mode_result = std::numeric_limits<fp_type>::quiet_NaN();
    size_t mode_freq = std::numeric_limits<size_t>::quiet_NaN()    ;
    { //mode
        auto it = begin,
             end_it = end;
        auto last_read = extr(*it);
        max = min = last_read;
        ++it;
        size_t freq = 1,
               count = 1;
        auto mode = last_read;

        for(; it != end_it; ++it) {
            auto curr_read = extr(*it);
            if(last_read == curr_read) {
                ++freq;
            } else {
                if(mode < curr_read) {
                    mode = curr_read;
                    count = freq;
                }
                freq = 0;
            }
            max = std::max<fp_type>(max, curr_read);
            min = std::min<fp_type>(min, curr_read);
            last_read = curr_read;
        }

        if(count > 1) {
            mode_result = mode;
            mode_freq = count;
        }
    }


    r.entries.emplace_back("mean", mean);
    r.entries.emplace_back("median", median);
    r.entries.emplace_back("mode.value", mode_result);
    r.entries.emplace_back("mode.freq", mode_freq);
    r.entries.emplace_back("std.dev", std_dev);
    r.entries.emplace_back("min", min);
    r.entries.emplace_back("max", max);
    r.entries.emplace_back("count", dist);

    return r;
}



}}

#endif	/* SP_ALGO_STATS_HPP */

