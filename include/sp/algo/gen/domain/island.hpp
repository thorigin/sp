/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_ISLAND_HPP
#define	SP_ALGO_GEN_ISLAND_HPP

#include <memory>
#include <vector>
#include <utility>
#include "population.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Island
 *
 * Container of populations
 */
struct island {

    using value_type = population_ptr;
    using population_ptr_vec_type = std::vector<population_ptr>;
    using iterator = population_ptr_vec_type::iterator;
    using const_iterator = population_ptr_vec_type::const_iterator;

    island() : vec() {}

    population_ptr new_pop() {
        population_ptr pop(std::make_shared<population>());
        vec.push_back(pop);
        return pop;
    }

    population_ptr_vec_type new_pops(size_t count) {
        vec.reserve(count);
        for(size_t i = 0; i < count; ++i) {
            vec.emplace_back(new_pop());
        }
        return vec;
    }

    population_ptr add(population* p) {
        auto ptr = std::shared_ptr<population>(p);
        vec.push_back(ptr);
        return ptr;
    }

    size_t mem_size() const {
        return sizeof(*this) +
                std::accumulate(
                this->vec.begin(),
                this->vec.end(),
                size_t(0),
                [](size_t& total, const population_ptr& r) {
            return total + r->mem_size();
        });
    }

    population_ptr_vec_type populations() {
        return vec;
    }

    population_ptr_vec_type populations() const {
        return vec;
    }

    size_t size() {
        return vec.size();
    }

    size_t size() const {
        return vec.size();
    }

    population_ptr operator[](size_t id) {
        return this->vec[id];
    };

    const population_ptr operator[](size_t id) const {
        return this->vec[id];
    };

    iterator begin() {
        return vec.begin();
    }

    iterator end() {
        return vec.end();
    }

    const_iterator begin() const {
        return vec.begin();
    }

    const_iterator end() const {
        return vec.end();
    }

    const_iterator cbegin() const {
        return vec.cbegin();
    }

    const_iterator cend() const {
        return vec.cend();
    }

protected:

    population_ptr_vec_type vec;

};

/**
 * \brief island pointer typedef
 */
using island_ptr = std::shared_ptr<island>;

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_ISLAND_HPP */

