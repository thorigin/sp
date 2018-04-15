/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */


#ifndef SP_UTIL_PROGRESS_HPP
#define	SP_UTIL_PROGRESS_HPP

#include <iostream>

SP_UTIL_NAMESPACE_BEGIN

/**
 * \file Provides a progress utility class to help maintain visual feedback
 *       during long console operations
 */

/**
 * \brief Progress utility class
 *
 * overloads the output stream operator and writes out a visual indicator which
 * on multiple calls will overwrite the previously printed out line.
 */
struct progress {

    progress(double total_) : total(total_) {}

    friend std::ostream& operator<<(std::ostream& os, const progress& p) {
        size_t bar_width = 50;
        double comp = (p.prog)/p.total;
        size_t current_pos = comp * bar_width;
        std::cout << "[";
        for (size_t i = 0; i < bar_width; ++i) {
            if (i < current_pos) {
                std::cout << "=";
            } else if (i == current_pos){
                std::cout << ">";
            } else {
                std::cout << " ";
            }
        }
        os << "] " << size_t(comp * 100.0f) << "%\r";
        os.flush();
        return os;
    }

    void update(double p) {
        prog = p;
    }

private:
    double prog;
    double total;
};


SP_UTIL_NAMESPACE_END


#endif  /* SP_UTIL_PROGRESS_HPP */