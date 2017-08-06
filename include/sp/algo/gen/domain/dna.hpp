/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_DNA_HPP
#define	SP_ALGO_GEN_DNA_HPP

#include <vector>
#include <utility>
#include <numeric>
#include "sp/util/attrmap.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief DNA - the genetic material made of one or more chromosomes
 */
struct dna {

    using value_type = bool;
    using chromosomes_vec_type = std::vector<chromosome*>;
    using iterator = chromosomes_vec_type::iterator;
    using const_iterator = chromosomes_vec_type::const_iterator;
    using attributes = util::attributes_map<std::string>;

    //no copy allowed
    dna(const dna& other)
        : vec(), eval_score(0.0f), lifetime_val(0), attrsmap() {
        vec.reserve(other.vec.size());
        for(auto    it = other.vec.begin(),
                    end = other.vec.end();
                    it != end;
                ++it) {
            vec.emplace_back(new chromosome(**it));
        }
    }

    dna(dna&&) = delete;
    //No assignment allowed (ptr only)
    dna& operator=(const dna& right)= delete;
    dna& operator=(dna&& right)= delete;

    dna()
        : vec(),
            eval_score(0.0),
            lifetime_val(0),
            attrsmap() {
    }

    virtual ~dna() {
        for(auto& ch : vec) {
            delete ch;
        }
    }

    void set_eval(float eval) {
        this->eval_score = eval;
    }

    float eval() const {
        return this->eval_score;
    }

    float eval() {
        return this->eval_score;
    }

    size_t lifetime() {
        return lifetime_val;
    }

    size_t lifetime() const {
        return lifetime_val;
    }

    sp_hot void increment_lifetime() {
        ++lifetime_val;
    }

    void add(chromosome* ch) {
        this->vec.push_back(ch);
    }

    size_t mem_size() const {
        return sizeof(*this) +
                std::accumulate(
                this->vec.begin(),
                this->vec.end(),
                size_t(0),
                [](size_t& total, const chromosome* r) {
            return total + r->mem_size();
        });
    }

    void reserve(size_t n) {
        vec.reserve(n);
    }

    size_t size() {
        return vec.size();
    }

    size_t size() const {
        return vec.size();
    }

    chromosome*& operator[](const size_t id) {
        return this->vec[id];
    };

    const chromosome* operator[](const size_t id) const {
        return this->vec[id];
    };

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

    sp_hot bool operator<(const dna& right) const {
        return right > *this;
    }

    sp_hot bool operator>(const dna& right) const {
        return (*this).eval() > right.eval();
    }

    attributes& attrs() {
        return attrsmap;
    }

    const attributes& attrs() const {
        return attrsmap;
    }

private:
    chromosomes_vec_type vec;
    float eval_score;
    size_t lifetime_val;
    attributes attrsmap;
};

using dna_ptr_pair = std::pair<dna*, dna*>;

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_DNA_HPP */

