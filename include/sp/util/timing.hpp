/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_UTIL_TIMING_HPP
#define SP_UTIL_TIMING_HPP

#include <iostream>
#include <iomanip>
#include <chrono>
#include "sp/config.hpp"

SP_UTIL_NAMESPACE_BEGIN

//Forward declared
struct scoped_timer;
std::ostream& operator<<(std::ostream& os, const scoped_timer& timer);

/**
 * \brief Scoped based timer class that helps with measuring the lifetime of
 * a scope
 */
struct scoped_timer {

    using clock = std::chrono::high_resolution_clock;

    scoped_timer(const std::string& name, bool printout = true) :
        timer_name(name),
        auto_printout(printout),
        start(clock::now()) {
    }

    scoped_timer(std::string&& name, bool printout = true) :
        timer_name(std::forward<std::string>(name)),
        auto_printout(printout),
        start(clock::now()) {
    }

    scoped_timer() :
        timer_name(),
        auto_printout(true),
        start(clock::now()) {
    }

    /**
     * \brief the number of seconds elapsed
     */
    inline double seconds() {
        return ((clock::now() - start) / std::chrono::nanoseconds(1)) / 1.0e9;
    }

    /**
     * \brief the number of seconds elapsed
     */
    inline double seconds() const {
        return ((clock::now() - start) / std::chrono::nanoseconds(1)) / 1.0e9;
    }

    /**
     * Divided the value by the amount of time elapsed
     * @param value
     * @return
     */
    template<typename T>
    inline double per_second(T value) {
        return value / seconds();
    }

    inline ~scoped_timer() {
        if (auto_printout) {
            std::cout << *this;
        }
    }

    inline const std::string& name() const {
        return timer_name;
    }

private:
    std::string timer_name;
    bool auto_printout;
    clock::time_point start;
    double elapsed;

};

/**
 * \brief ostream operator overload
 */
std::ostream& operator<<(std::ostream& os, const scoped_timer& timer) {
    auto flags = os.flags();
    os << std::fixed << std::setprecision(4);
    os  << "-------------------------------------\n"
        << ' ' << (timer.name().empty() ? "Default" : timer.name()) << " : "
        << timer.seconds() << " seconds\n"
        << "-------------------------------------\n";
    os.flags(flags);
    return os;
}

SP_UTIL_NAMESPACE_END

#endif /* SP_UTIL_TIMING_HPP */