/**
 * Copyright (C) Omar Thor <omarthoro\gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro\gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_LAYER_CONNECTIVITY_HPP
#define SP_ALGO_NN_LAYER_CONNECTIVITY_HPP

#include <array>
#include <vector>
#include "layer.hpp"

SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * \file Implements various connectivity methods
 */


/**
 * \brief Group Connectivity of convolution
 */
template<size_t Groups, size_t Rows, size_t Cols>
struct ngroups_connectivity {

    /**
     * Groups in the sparsity tables
     */
    constexpr static size_t groups = Groups;

    /**
     * Rows in the sparsity table
     */
    constexpr static size_t rows = Rows;

    /**
     * Columns in the sparsity table
     */
    constexpr static size_t cols = Cols;

    /**
     * Size of the sparsity table
     */
    constexpr static size_t size = rows * cols;

    ngroups_connectivity() : table(initialize()) {}
    ngroups_connectivity(const ngroups_connectivity&) = default;
    ngroups_connectivity(ngroups_connectivity&&) = default;
    ngroups_connectivity& operator=(const ngroups_connectivity&) = default;
    ngroups_connectivity& operator=(ngroups_connectivity&&) = default;

    inline bool operator()(const size_t& x, const size_t& y) {
        return table[x * rows + y];
    }

    std::array<bool, size> initialize() {
        std::array<bool, size> res;
        auto row_group = rows / groups;
        auto col_group = cols / groups;
        for(size_t g = 0; g < groups; g++) {
            for(size_t r = 0; r < row_group; r++) {
                for(size_t c = 0; c < col_group; c++) {
                    res[(r + g * row_group) * cols + c + g * col_group] = true;
                }
            }
        }
        return res;
    }

    std::array<bool, size> table;
};

/**
 * \brief Enables full connections on every input/ouput
 *
 */
struct full_connectivity {
    inline bool operator()(const size_t&, const size_t&) {
        return true;
    }
};

/**
 * @brief Creates a connectivity table based on the input table
 *
 * @param Values the list of values (in a sequence), number of values must
 *        equal Rows * Cols
 */
template<size_t Rows, size_t Cols, bool ... Values>
struct table_connectivity {

    static_assert(sizeof...(Values) == Rows*Cols, "Size of values provided matched dimensions specified");

    /**
     * Rows in the sparsity table
     */
    constexpr static size_t rows = Rows;

    /**
     * Columns in the sparsity table
     */
    constexpr static size_t cols = Cols;

    /**
     * Size of the sparsity table
     */
    constexpr static size_t size = rows * cols;

    table_connectivity() : table({Values...}) {}

    inline bool operator()(const size_t& x, const size_t& y) {
        return table[y * cols + x];
    }

    std::vector<char> table;

};

SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_LAYER_FULLY_CONNECTED_HPP */

