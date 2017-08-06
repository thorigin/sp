/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_POPULATION_HPP
#define SP_ALGO_GEN_POPULATION_HPP

#include <vector>
#include <algorithm>
#include <memory>
#include "sp/config.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Population
 *
 * Container of DNAs
 */
struct population {

    using value_type = dna*;
    using dna_vec_type = std::vector<dna*>;
    using iterator = dna_vec_type::iterator;
    using const_iterator = dna_vec_type::const_iterator;

    population() : vec(), evols(0) { }

    population(const population & other) = delete;
    population(population && other) = delete;
    population& operator=(population&& right) = delete;
    population& operator=(const population& right) = delete;

    virtual ~population() {
        for(dna* ptr : this->vec) {
            delete ptr;
        }
    }

    sp_hot size_t size() const {
        return this->vec.size();
    }

    size_t mem_size() const {
        return sizeof(*this) + std::accumulate(
                this->vec.begin(),
                this->vec.end(),
                size_t(0),
                [](size_t& total, const dna* r) {
            return total + r->mem_size();
        });
    }

    sp_hot void reserve(size_t n) {
        this->vec.reserve(n);
    }

    /**
     * Returns a pointer to a {chromosome_value_type} which could be deleted at any time
     *
     * \param id
     * \return
     */
    sp_hot dna* operator[](const size_t& index) {
        return this->vec[index];
    };

    sp_hot const dna* operator[](const size_t& index) const {
        return this->vec[index];
    };

    /**
     * \brief Remove elements from the top
     *
     * Where the population (0... n) becomes (0 ... n-c), where c
     * is the number of elements to remove
     *
     * This function is merely a convenience function to modify the population,
     * and it must be called at a time when the population is ordered
     *
     *
     * Assumes that the population is in a well-ordered state at the time of
     * execution
     *
     * \param elementCount the number of chromosomes to remove
     */
    sp_hot void remove_from_bottom(size_t elementCount) {
        auto    size = this->vec.size(),
                from = size - elementCount;
        for(auto it = this->vec.begin() + from, end = this->vec.end(); it != end; ++it) {
            delete *it;
        }
        this->vec.erase(this->vec.begin() + from, this->vec.end());
    }

    sp_hot void add(dna* value) {
        this->vec.push_back(value);
    }

    /**
     * \brief Inserts all elements from one container this population
     *
     * Warning: This will mean the DNAs now reside in both containers after this operation
     * \param container
     */
    template<typename Container>
    sp_hot void insert(const Container& container) {
        this->vec.reserve(this->vec.size() + container.size());
        this->vec.insert(this->vec.end(), container.begin(), container.end());
    }

    /**
     * Inserts all elements from one container to another
     *
     * Warning: This will mean the DNAs now reside in both containers after this operation
     * \param begin
     * \param end
     */
    template<typename Iterator>
    sp_hot void insert(Iterator begin, Iterator end) {
        auto size = std::distance(begin, end);
        this->vec.reserve(this->vec.size() + size);
        this->vec.insert(this->vec.end(), begin, end);
    }

    /**
     * \brief Copies all DNAs from one container into this population
     *
     * \param cont
     */
    template<typename Container>
    sp_hot void copy(const Container& cont) {
        this->vec.reserve(this->vec.size() + cont.size());
        this->vec.insert(this->vec.end(), cont.begin(), cont.end());
        std::transform(std::begin(cont), std::end(cont), this->vec.end(), [](const dna* d) { return new dna(*d); });
    }

    template<typename Iterator>
    sp_hot void copy(Iterator begin, Iterator end) {
        auto size = std::distance(begin, end);
        this->vec.reserve(this->vec.size() + size);
        std::transform(begin, end, this->vec.end(), [](const dna* d) { return new dna(*d); });
    }

    sp_hot void incr_evolutions() {
        ++this->evols;
    }

    sp_hot size_t evolutions() {
        return this->evols;
    }

    sp_hot size_t evolutions() const {
        return this->evols;
    }

    sp_hot iterator begin() {
        return vec.begin();
    }

    sp_hot iterator end() {
        return vec.end();
    }

    sp_hot const_iterator begin() const {
        return vec.begin();
    }

    sp_hot const_iterator end() const {
        return vec.end();
    }

    sp_hot const_iterator cbegin() const {
        return vec.cbegin();
    }

    sp_hot const_iterator cend() const {
        return vec.cend();
    }

protected:
    dna_vec_type vec;
    size_t evols;
};

using population_ptr = std::shared_ptr<population>;


SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_POPULATION_HPP */

