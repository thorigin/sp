/*
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_CHROMOSOME_HPP
#define	SP_ALGO_GEN_CHROMOSOME_HPP

#include <initializer_list>
#include "sp/util/alloc.hpp"
#include "sp/config.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Chromosome
 */
struct chromosome {

    chromosome(const chromosome& other)
        : storage(util::aligned_alloc<bool*>(other.length)),
            length(other.length) {
        std::copy(other.storage, other.storage + other.length, storage);
    }

    //\todo keep deleted or implement
    chromosome(chromosome&&) = delete;
    //No assignment allowed, or needed
    chromosome& operator=(const chromosome& right)= delete;
    chromosome& operator=(chromosome&& right)= delete;

    chromosome(std::initializer_list<bool> init_list)
        : storage(util::aligned_alloc<bool*>(init_list.size())),
            length(init_list.size()) {
        std::copy(init_list.begin(), init_list.end(), storage);
    }

    chromosome(const size_t& size)
        : storage(util::aligned_alloc<bool*>(size)),
            length(size) {}

    virtual ~chromosome() {
        util::aligned_free(storage);
    }

    size_t size() {
        return length;
    }

    size_t size() const {
        return length;
    }

    inline bool& operator[](size_t index) {
        return this->storage[index];
    }

    inline bool operator[](size_t index) const {
        return this->storage[index];
    }

    inline void set_value(size_t index, bool val) {
        this->storage[index] = val;
    }

    sp_hot void flip(const size_t& index) {
        this->storage[index] = !this->storage[index];
    }

    size_t mem_size() const {
        return sizeof(*this) + (sizeof(*storage) * this->length);
    }

private:
    bool* storage;
    size_t length;
};


SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_CHROMOSOME_HPP */

