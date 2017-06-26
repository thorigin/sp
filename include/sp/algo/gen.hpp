/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_HPP
#define	SP_ALGO_GEN_HPP

#include "sp/util/alloc.hpp"
#include "sp/util/rand.hpp"
#include "sp/util/hints.hpp"
#include "sp/util/attrmap.hpp"

//#include <parallel/algorithm>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <functional>
#include <climits>
#include <ratio>
#include <map>
#include <vector>
#include <boost/assert.hpp>
#include <iostream>
#include <boost/random.hpp>
#include <boost/integer.hpp>
#include <boost/fusion/view/flatten_view.hpp>
#include <boost/fusion/include/flatten_view.hpp>


namespace sp { namespace algo { 

/**
 * \brief Provides for the fundamentals of genetic algorithms and genetic programming
 * concepts of evolutionary computing
 *
 * "Extinction is the rule. Survival is the exception."
 *  ― Carl Sagan
 *
 * @author Omar Thor <omarthoro@gmail.com>
 */    
namespace gen {

/**
 * \brief Chromosome
 */
struct chromosome {

    chromosome(const chromosome& other)
        :   storage(util::aligned_alloc<bool*>(other.length)),
            length(other.length) {
        std::copy(other.storage, other.storage + other.length, storage);
    }

    //\todo keep deleted or implement
    chromosome(chromosome&&) = delete;
    //No assignment allowed, or needed
    chromosome& operator=(const chromosome& right)= delete;
    chromosome& operator=(chromosome&& right)= delete;

    chromosome(std::initializer_list<bool> init_list)
        :   storage(util::aligned_alloc<bool*>(init_list.size())),
            length(init_list.size()) {
        std::copy(init_list.begin(), init_list.end(), storage);
    }

    chromosome(const size_t& size)
        :   storage(util::aligned_alloc<bool*>(size)),
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
        :   vec(),
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

/**
 * \brief DNA comparator
 *
 * Compares the evaluation (eval) of each DNA
 * Where a higher eval value has higher precedence
 */
struct highest_dna_eval_comparator {

    sp_hot bool operator()(const dna* const l, const dna* const r) const {
        return l->eval() > r->eval();
    }
};

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

using island_ptr = std::shared_ptr<island>;



/**
 * \brief Evolution stop context
 *
 * Works on a per population basis
 *
 */
struct evolution_stop_context {

    evolution_stop_context(const population* const pop) : p(pop) {}

    float highest_eval() const  {
        float highest = -1;
        size_t len = to;
        for(size_t i = from; i < len; ++i) {
            float current = (*p)[i]->eval();
            if(current > highest)
                highest = current;
        }
        return highest;
    }

    size_t evol_count() const {
        return p->evolutions();
    }

    float average_fit() const  {
        float total = std::accumulate(
            p->begin() + from,
            p->begin() + to,
            float(0),
            [](float total, const dna* const d) {
                return total + d->eval();
            }
        );
        return (total / (to-from));
    }

    /**
     * Sets the range of DNAs to consider for evaluation context
     *
     *
     * @param from
     * @param to
     */
    void set_valid_range(size_t from, size_t to) {
        this->from = from;
        this->to = to;
    }
private:
    const population* const p;
    size_t from, to;
};


template<
    typename MultiplicationStrategy,
    typename PopulationControl,
    typename MigrationModel,
    typename RandomEngine = boost::random::mt11213b
>
struct model {

    using multiplication_strategy_type = MultiplicationStrategy;
    using population_control_type = PopulationControl;
    using random_engine_type = RandomEngine;

    using island_ptr_vec_type = std::vector<island_ptr>;
    using iterator = island_ptr_vec_type::iterator;
    using const_iterator = island_ptr_vec_type::const_iterator;
    using random_type = util::rand_util<random_engine_type>;

    model()
        :   mult_strategy(),
            pop_ctrl_strategy(),
            vec(),
            rand() {
    }

    island_ptr new_island() {
        island_ptr pop(std::make_shared<island>());
        vec.push_back(pop);
        return pop;
    }

    island_ptr_vec_type new_islands(size_t count) {
        island_ptr_vec_type vec;
        vec.reserve(count);
        for(size_t i = 0; i < count; ++i) {
            vec.emplace_back(new_island());
        }
        return vec;
    }

    random_type& random() {
        return rand;
    }

    void seed(  size_t dna_count,
                std::vector<size_t> chromosome_sizes,
                population_ptr p) {
        seed(dna_count, chromosome_sizes, p.get());
    }\

    void seed(  size_t dna_count,
                std::vector<size_t> chromosome_sizes,
                std::vector<population_ptr> pops) {
        for(auto& p : pops) {
            seed(dna_count, chromosome_sizes, p);
        }
    }

    void seed(  size_t dna_count,
                std::vector<size_t> chromosome_sizes,
                population* p) {

        p->reserve(dna_count);
        for(size_t di = 0; di < dna_count; ++di) {
            //create chromosomes
            dna* d = create_dna(chromosome_sizes);
            p->add(d);
        }
    }

    /**
     * \brief Creates an empty DNA
     * \return
     */
    sp_hot dna* create_dna() {
        return new dna();
    }

    /**
     * Creates a  dna strand
     * \param ch_bits
     * \return
     */
    sp_hot dna* create_dna(const std::vector<size_t>& chromosome_sizes, bool randomize = true) {
        dna* d = create_dna();
        d->reserve(chromosome_sizes.size());
        for(auto    it = chromosome_sizes.begin(),
                    end = chromosome_sizes.end();
                    it != end;
                    ++it) {
            auto ch = create_chromosome(*it, randomize);
            d->add(ch);
        }
        return d;
    }

    /**
     * \brief Creates (optionally randomized) DNA with the specified chromosome
     * count and size
     *
     * \param chromosome_sizes
     * \param randomize (optional)
     * \return
     */
    sp_hot dna* create_dna(std::vector<size_t>&& chromosome_sizes, bool randomize = true) {
        dna* d = new dna();
        d->reserve(chromosome_sizes.size());
        for(auto    it = chromosome_sizes.begin(),
                    end = chromosome_sizes.end();
                    it != end;
                    ++it) {
            auto ch = create_chromosome(*it, randomize);
            d->add(ch);
        }
        return d;
    }

    /**
     * \brief Creates (optionally randomized) DNA with the specified chromosome
     * count and size
     *
     * \param chromosome_sizes
     * \param randomize (optional)
     * \return
     */
    sp_hot chromosome* create_chromosome(const size_t ch_bits, bool randomize = true) {
        constexpr size_t random_int_bits = sizeof (typename random_type::result_type) * CHAR_BIT;
        const size_t ch_parts = ch_bits / random_int_bits;
        const size_t ch_rem = ch_bits%random_int_bits;
        chromosome* ch = new chromosome(ch_bits);
        if(randomize) {
            for(size_t ch_part_idx = 0; ch_part_idx < ch_parts; ++ch_part_idx) {
                auto rnd_int = this->rand.rand_integral();
                for (size_t bit_idx = 0; bit_idx < random_int_bits; ++bit_idx) {
                    ch->set_value(
                        random_int_bits*ch_part_idx +
                        bit_idx, (rnd_int >> bit_idx) & 0x1
                    );
                }
            }
            if(ch_rem != 0) {
                auto rnd_int = this->rand.rand_integral();
                for(size_t ch_r_i = 0; ch_r_i < ch_rem; ++ch_r_i) {
                    ch->set_value(
                        random_int_bits*ch_parts +
                        ch_r_i, (rnd_int >> ch_r_i) & 0x1
                    );
                }
            }
        }
        return ch;
    }

    /**
     * \brief Evolve a collection of islands, i.e. a vector or a set.
     * \param cont
     * \param predicate
     * \return
     */
    template<typename Container, typename Predicate>
    std::enable_if_t<
        std::is_same<typename Container::value_type, island_ptr>::value,
        void
    >
    evolve(Container& cont, Predicate predicate) {
        for(auto
                it = cont.begin(),
                end = cont.end();
                it != end;
                ++it) {
            evolve((**it), predicate);
        }
    }

    /**
     * \brief Evolve a collection of populations , i.e. an  island, a vector, or a set.
     * \param cont
     * \param predicate
     * \return
     */
    template<typename Container, typename Predicate>
    std::enable_if_t<
        std::is_same<typename Container::value_type, population_ptr>::value
    >
    evolve(Container& cont, Predicate predicate) {
        evolve(cont.begin(), cont.end(), predicate);
    }

    /**
     * \brief Evolve a range of populations
     * \param it
     * \param end
     * \param predicate
     */
    template<typename Iterator, typename Predicate>
    void evolve(Iterator it, Iterator end, Predicate predicate) {
        for(; it != end; ++it) {
            evolve(*it, predicate);
        }
    }

    template<typename Predicate>
    void evolve(population_ptr& pop, Predicate predicate) {
        evolve(pop.get(), std::forward<Predicate>(predicate));
    }

    template<typename Predicate>
    sp_no_inline void evolve(population* p, Predicate predicate) {
        evolution_stop_context stop_ctx(p);
        do {
            size_t  add_ct = pop_ctrl_strategy.add_count(this, p),
                    prune_ct = pop_ctrl_strategy.prune_count(this, p);


            pop_ctrl_strategy.evaluate(this, p, p->size()-prune_ct, p->size());

            if(p->evolutions() % 1000 == 0) {
                std::cout << sp::algo::make_stats("Evolutions", *p, [](const dna* const d) { return d->eval(); });
            }

            if(population_control_type::prune_before_multiplication) {
                p->remove_from_bottom(prune_ct);
            }

            //Do not include pruned into reserve, as they're already removed,
            //and won't need any extra space
            p->reserve(p->size() + add_ct);

            //Increment before multiplying
            increment_counters(p);

            mult_strategy(this, p, add_ct);

            if(!population_control_type::prune_before_multiplication) {
                p->remove_from_bottom(prune_ct);
            }

            stop_ctx.set_valid_range(0, p->size()-prune_ct);

        } while(!predicate(stop_ctx));
    }

    size_t size() {
        return vec.size();
    }

    size_t size() const {
        return vec.size();
    }

    population_ptr* operator[](size_t& id) {
        return this->vec[id];
    };

    const dna* operator[](size_t& id) const {
        return this->vec[id];
    };

    iterator begin() {
        return vec.end();
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

    sp_hot void increment_counters(population* const p) {
        p->incr_evolutions();
        for(auto it = p->begin(), end = p->end(); it != end; ++it) {
            (*it)->increment_lifetime();
        }
    }

    multiplication_strategy_type mult_strategy;
    population_control_type pop_ctrl_strategy;
    island_ptr_vec_type vec;
    random_type rand;

};

/**
 * \brief Macro helper that makes a boolean expression with a context bound variable
 * ctx of type evolution_stop_context available for shorthand expressions,
 * simplifies lambda generations into easily readable condition.
 *
 * For example:
 * Given a model variable m, and population variable pop
 * then the evolve call simplifies to:
 *
 *      m.evolve(pop, sp_evolve_until(ctx.evol_count() >= 1000))
 *
 * \param expression boolean expression
 * \return result of expression
 */
#define sp_evolve_until(expression) [](const auto& ctx) { return (expression); }


template<typename Container>
auto mem_size(Container& cont) {
    return sizeof(cont) + std::accumulate(
            cont.begin(),
            cont.end(),
            size_t(0),
            [](size_t& total, const island_ptr& r) {
        return total + r->mem_size();
    });
}

/**
 * \brief Population Evaluator
 *
 * Responsible assigning a fitness score to every individual chromosome in the
 * population using the specified Fitness evaluator to evaluate the fitness
 * score of each chromosome in the population.
 * \param Fitness_evaluator_type the fitnesss evaluator responsible for
 */
template<typename FitnessEvaluator, typename Derived>
struct population_evaluator {

    using fitness_evaluator_type = FitnessEvaluator ;

    using derived_type = Derived ;

    template<typename Model>
    sp_hot
    void operator()(Model* m, population* pop, size_t from, size_t to) {
        derived().impl(m, pop, from, to);
    }

    /**
     * CRTP helper
     */
    derived_type& derived() {
        return *static_cast<derived_type*>(this);
    }

};


/**
 * \brief Zero fitness Evaluator
 *
 * Simply returns a fitness score of 0 for every dna
 */
struct zero_fitness_evaluator {

    float operator()(const dna* const d) const {
        return 0.0f;
    }

};

/**
 * \brief Population Evaluator
 *
 */
template<typename FitnessEvaluator>
struct default_population_evaluator
    :   population_evaluator<
            FitnessEvaluator,
            default_population_evaluator<FitnessEvaluator>
        > {

    using fitness_evaluator_type = FitnessEvaluator;

    template<typename Model>
    sp_hot void impl(Model* m, population* pop, size_t from, size_t to) {
        FitnessEvaluator evaluator;
        for(size_t i = from; i < to; ++i) {
            dna* c = (*pop)[i];
            c->set_eval(evaluator(c));
        }
        std::stable_sort(
            pop->begin(),
            pop->end(),
            highest_dna_eval_comparator()
        );
    }

};

/**
 * \param Derived_type
 * \param Fitness_evaluator_type
 * \param Population_evaluator_type
 * \param Prune_before_multiplication Whether or not DNAs are removed from the pool
 * before or after multiplication. Note: Doing it before can be more memory efficient.
 */
template<
    typename Derived,
    typename FitnessEvaluator,
    typename Population_evaluator_type = default_population_evaluator<FitnessEvaluator>,
    bool Prune_before_multiplication = true
>
struct population_control {

    using derived_type = Derived;
    using fitness_evaluator_type = FitnessEvaluator;
    using population_evaluator_type = Population_evaluator_type;

    constexpr static bool prune_before_multiplication = Prune_before_multiplication;

    template<typename Model>
    sp_hot void evaluate(Model* m, population* p, size_t from, size_t to) {
        derived().evaluate_impl(m, p, from, to);
    }

    template<typename Model>
    sp_hot size_t add_count(Model* m, population* p) {
        return derived().add_count_impl(m, p);
    }

    template<typename Model>
    sp_hot size_t prune_count(Model* m, population* p) {
        return derived().prune_count_impl(m, p);
    }

    /**
     * CRTP helper
     */
    derived_type& derived() {
        return *static_cast<derived_type*>(this);
    }
};

/**
 * \brief Population Growth by a ratio (percentage)
 *
 * Note: Growth and replace ratios are greedy, i.e. always round up. This is in
 * part due to low population size yielding no change if ratio is very low
 * \tparam GrowthRatio
 * \tparam ReplaceRatio
 * \tparam FitnessEvaluator the fitness evaluator type
 * \tparam PopulationEvaluator
 * \tparam PruneBeforeMultiplication
 * \tparam RoundResultsUp (optional) default true
 */
template<
    typename GrowthRatio,
    typename ReplaceRatio,
    typename FitnessEvaluator,
    typename PopulationEvaluator = default_population_evaluator<FitnessEvaluator>,
    bool PruneBeforeMultiplication = true,
    bool RoundResultsUp = true
>
struct ratio_population_control
    :   population_control<
            ratio_population_control<
                GrowthRatio,
                ReplaceRatio,
                FitnessEvaluator,
                PopulationEvaluator,
                PruneBeforeMultiplication
            >,
            FitnessEvaluator,
            PopulationEvaluator,
            PruneBeforeMultiplication
        > {

    using growth_ratio = GrowthRatio;
    using replace_ratio = ReplaceRatio;

    static constexpr bool round_result_up = RoundResultsUp;

    static constexpr float growth_perc = static_cast<float>(growth_ratio::num) / static_cast<float>(growth_ratio::den);
    static_assert(growth_perc >= 0.0f, "Growth_ratio must be greater or equal to zero");
    static constexpr float replace_perc = static_cast<float>(replace_ratio::num) / static_cast<float>(replace_ratio::den);
    static_assert(replace_perc >= 0.0f, "Replace_ratio must be greater or equal to zero");

    ratio_population_control() : evaluator() {}

    template<typename Model>
    sp_hot void evaluate_impl(Model* m, population* p, size_t from, size_t to) {
        evaluator(m, p, from, to);
    }

    template<typename Model>
    sp_hot size_t add_count_impl(Model*, population* p) {
        if(round_result_up) {
            return std::ceil(p->size() * (replace_perc + growth_perc));
        } else {
            return p->size() * (replace_perc + growth_perc);
        }
    }

    template<typename Model>
    sp_hot size_t prune_count_impl(Model* m, population* p) {
        if(round_result_up) {
            return std::ceil(p->size() * replace_perc);
        } else {
            return p->size() * replace_perc;
        }
    }

    PopulationEvaluator evaluator;
};

/**
 * \brief Zero population control
 *
 */
template<
    typename Fitness_evaluator_type,
    typename Population_evaluator_type = default_population_evaluator<Fitness_evaluator_type>,
    bool Prune_before_multiplication = true
>
struct zero_population_control
    :   population_control<
            zero_population_control<
                Fitness_evaluator_type,
                Population_evaluator_type,
                Prune_before_multiplication
            >,
            Fitness_evaluator_type,
            Population_evaluator_type,
            Prune_before_multiplication
        > {


    template<typename Model>
    sp_hot size_t add_count_impl(Model*, population* p) {
        return 0;
    }

    template<typename Model>
    sp_hot size_t prune_count_impl(Model* m, population* p) {
        return 0;
    }
};


/**
 * \brief Multiplication strategy
 *
 * Encapsulates the various details of multiplication of the model
 *
 * For complete implementations, see mitosis_stratety, and meiosis_strategy
 *
 * Multiplication strategies take the model as an argument, the population to
 * perform multiplication within, and a multi_count parameter specying how many
 * multiplications to perform. Results are added to the population on
 * completion.
 *
 * Adds the new resulting chromosomes at the end of the population
 *
 */
template<typename ... Operators>
struct multiplication_pipeline_strategy {

    template<typename Model>
    sp_hot void operator()(Model* m, population* p, const size_t& multi_count) {
        repeat<Model, Operators...>(m, p, multi_count);
    }

private:

    static_assert(sizeof...(Operators) >= 2, "Multiplication Pipeline strategy should consist of at least one selection and one recombination operations");

    template<typename Model, typename First, typename ... Others>
    sp_hot void repeat(Model* m, population* p, const size_t& multi_count) {
        for(size_t i = 0; i < multi_count; ++i) {
            auto res = pipe<Model, Others...>(m, p, multi_count, First()(m, p, multi_count, i));
            p->add(res);
        }
    }

    template<typename Model, typename First, typename Second, typename ... Others>
    sp_hot typename First::result_type pipe(
        Model* m,
        population* p,
        const size_t& multi_count,
        typename First::operand_type input) {
        return pipe<Model, Second, Others...>(m, p, multi_count, First()(m, p, multi_count, input));
    }

    //Base case
    template<typename Model, typename Last>
    sp_hot typename Last::result_type pipe(
        Model* m,
        population* p,
        const size_t& multi_count,
        typename Last::operand_type input) {
        return Last()(m, p, multi_count, input);
    }

};

/**
 * \brief Multiplication Pipeline Operation
 *
 */
template<typename Operand, typename Result, typename Derived_type>
struct multiplication_pipeline_op {

    using operand_type = Operand;
    using result_type = Result;
    using derived_type = Derived_type;


    template<typename Model>
    result_type operator()(Model* m, population* p, const size_t& multi_count, operand_type operand) {
        return derived().impl(m, p, multi_count, operand);
    }

    /**
     * CRTP helper
     */
    derived_type & derived() {
        return *static_cast<derived_type*>(this);
    }
};

template<typename Operand>
struct noop_unary_pipeline_op
    : multiplication_pipeline_op<Operand, Operand, noop_unary_pipeline_op<Operand>> {

    using operand_type = Operand;
    using result_type = Operand;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type operand) {
        return operand;
    }
};

/**
 * \brief Selection Strategy
 *
 * The method in which to select the DNAs to be considered for propagation
 */
template<typename Result, typename Derived_type>
struct selection_op
    :   multiplication_pipeline_op<
            const size_t&, //pair id
            Result,
            Derived_type
        > {
    using operand_type = const size_t&;
    using result_type = Result;
};

template<typename Derived>
struct unary_selection_op
    :   selection_op<
            dna*,
            Derived
        > {

    using operand_type = typename unary_selection_op::operand_type;

    using result_type = typename unary_selection_op::result_type;
};

template<typename Derived>
struct binary_selection_strategy
    :   selection_op<
            dna_ptr_pair,
            Derived
        > {

    using operand_type = typename binary_selection_strategy::operand_type;

    using result_type = typename binary_selection_strategy::result_type;

};

struct zero_selection_op : unary_selection_op<zero_selection_op> {

    using operand_type = typename zero_selection_op::operand_type;
    using result_type = typename zero_selection_op::result_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type id) {
        return nullptr;
    }
};


/**
 * \brief Tournament Selection Strategy
 *
 */
 struct tournament_unary_selection_op : unary_selection_op<tournament_unary_selection_op> {

    using operand_type = typename tournament_unary_selection_op::operand_type;
    using result_type = typename tournament_unary_selection_op::result_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type pair_id) {
        return (*p)[pair_id];
    }

};

/**
 * \brief Tournament Selection
 *
 *
 * This strategy considers the highest scoring fitness
 */
 struct tournament_binary_selection_op : binary_selection_strategy<tournament_binary_selection_op> {

    using operand_type = typename tournament_binary_selection_op::operand_type;
    using result_type = typename tournament_binary_selection_op::result_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type pair_id) {
        //Handle cases where pair_id has gone beyond half the population size,
        //wraps around as a solution
        const operand_type size = p->size();
        size_t first_idx = (pair_id*2 + 0) % size,
        second_idx = (pair_id*2 + 1) % size;
        dna* first = (*p)[first_idx];
        dna* second = (*p)[second_idx];
        return dna_ptr_pair(first, second);
    }

};

/**
 * \brief Fitness Proportionate Binary Selection
 *
 * linear O(n))
 *
 * Also known as Roulette Wheel Selection
 */
struct fitness_proportionate_binary_selection_op : binary_selection_strategy<fitness_proportionate_binary_selection_op> {

    using operand_type = typename fitness_proportionate_binary_selection_op::operand_type;
    using result_type = typename fitness_proportionate_binary_selection_op::result_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type pair_id) {

        dna* first = nullptr;
        dna* second = nullptr;

        auto sum_fitness = std::accumulate(p->begin(), p->end(), float(0), [](const float& sum, const dna* d) { return sum + d->eval(); });
        float value = m->random().rand_float() * sum_fitness;

        //select the first
        for(auto it = p->begin(), end = p->end(); it != end; ++it) {
            auto d = *it;;
            value -= d->eval();
            if(value <= 0) {
                first = d;
                break;
            }
        }
        //select the second
        float value2 = m->random().rand_float() * sum_fitness;

        for(auto it = p->begin(), end = p->end(); it != end; ++it) {
            auto d = *it;
            value2 -= d->eval();
            //if value is less and not the first value
            if(value2 <= 0 && d != first) {
                second = d;
                break;
            }
        }

        if(!first) {
           first = (*p)[p->size()-2];
        }
        if(!second) {
            second = (*p)[p->size()-1];
        }

        return dna_ptr_pair(first, second);
    }

};

/**
 * \brief Fitness Proportionate Unary Selection
 *
 * linear O(n))
 *
 * Also known as Roulette Wheel Selection
 */
struct fitness_proportionate_unary_selection_op : unary_selection_op<fitness_proportionate_unary_selection_op> {

    using operand_type = typename fitness_proportionate_unary_selection_op::operand_type;
    using result_type = typename fitness_proportionate_unary_selection_op::result_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type pair_id) {
        dna* selection = nullptr;

        auto sum_fitness = std::accumulate(p->begin(), p->end(), float(0), [](const float& sum, const dna* d) { return sum + d->eval(); });
        float value = m->random().rand_float() * sum_fitness;

        for(auto it = p->begin(), end = p->end(); it != end; ++it) {
            value -= (*it)->eval();
            if(value <= 0) {
                selection = *it;
                break;
            }
        }

        if(!selection) {
           selection = (*p)[p->size()-1];
        }

        return selection;
    }

};

/**
 * \brief Simulated Binary Fission
 *
 *
 * Note: The name binary fission may be confusing, as it returns
 * only a single DNA, whereas the original simply remains in the pool
 */
struct simulated_binary_fission
    :   multiplication_pipeline_op<
            dna*,
            dna*,
            simulated_binary_fission
        > {

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type d) {
        return new dna(*d);
    }
};

/**
 * \brief Recombination Operator
 *
 * Possible aspects of which this operator covers:
 * * Crossover
 * * Gene Conversion
 * * Nonhomologous recombination
 *
 * Takes two or more chromosomes and combines them into an entirely new
 * resulting genetic combination depending on the implementation
 *
 */
template<typename Operand, typename Derived>
struct recombination_op
    :   multiplication_pipeline_op<Operand, dna*, Derived> {
    using operand_type = typename recombination_op::operand_type;
    using result_type = typename recombination_op::result_type;
};


/**
 * \brief Zero Recombination Op
 *
 * No-op
 */
struct zero_recombination_op
    :   recombination_op<dna*, zero_recombination_op> {

    using operand_type = typename recombination_op<dna*, zero_recombination_op>::operand_type;
    using result_type = typename recombination_op<dna*, zero_recombination_op>::result_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type pair) {
        return nullptr;
    }
};



/**
 * \brief Binary Striping Recombination Operations
 *
 * Combines two DNAs by striping their genetic material together
 *
 *
 * This is a very generic method of recombining two DNAs together
 *
 * First, it creates a new child DNA, next it copies the chromosome
 * \param Striping_size the striping length
 */
template<std::intmax_t StripingSize = 1>
struct binary_striping_recombination_op
    :   recombination_op<dna_ptr_pair, binary_striping_recombination_op<StripingSize>> {

    static constexpr std::intmax_t striping_size = StripingSize;
    static constexpr std::intmax_t double_striping_size = striping_size * 2;


    using operand_type = typename binary_striping_recombination_op::operand_type;
    using result_type = typename binary_striping_recombination_op::result_type;

    static_assert(striping_size > 0, "Striping size must be greater than 1");

    template<typename Model>
    dna* impl(Model* m, population* p, const size_t& multi_count, dna_ptr_pair pair) {
        dna* d = new dna();
        auto dna_len_1 = pair.first->size();
        auto dna_len_2 = pair.second->size();
        auto chromosome_count = std::max(dna_len_1, dna_len_2);
        d->reserve(chromosome_count);
        for(size_t new_chromosome_idx = 0; new_chromosome_idx < chromosome_count; ++new_chromosome_idx) {
            const chromosome* first = (*pair.first)[new_chromosome_idx];
            const chromosome* second = (*pair.second)[new_chromosome_idx];

            auto ch_bit_len_fir = first->size();
            auto ch_bit_len_sec = second->size();
            auto ch_bit_len_max = std::max(ch_bit_len_fir, ch_bit_len_sec);
            auto ch_bit_len_div = ch_bit_len_max / double_striping_size;
            auto ch_bit_len_rem = ch_bit_len_max % double_striping_size;
            auto ch_bit_len_striped = ch_bit_len_div * double_striping_size;

            chromosome* new_ch = new chromosome(ch_bit_len_max);

            bool side_first = m->random().rand_integral(0, 1);

            for(size_t bit_idx = 0; bit_idx < ch_bit_len_striped; bit_idx+= double_striping_size) {
                if(side_first) {
                    for(size_t ch_striping_idx = 0; ch_striping_idx < striping_size; ++ch_striping_idx) {
                        (*new_ch)[bit_idx + ch_striping_idx] = (*first)[bit_idx + ch_striping_idx];
                    }
                    for(size_t ch_striping_idx = striping_size; ch_striping_idx < double_striping_size; ++ch_striping_idx) {
                        (*new_ch)[bit_idx + ch_striping_idx] = (*second)[bit_idx + ch_striping_idx];
                    }
                } else {
                    for(size_t ch_striping_idx = 0; ch_striping_idx < striping_size; ++ch_striping_idx) {
                        (*new_ch)[bit_idx + ch_striping_idx] = (*second)[bit_idx + ch_striping_idx];
                    }
                    for(size_t ch_striping_idx = striping_size; ch_striping_idx < double_striping_size; ++ch_striping_idx) {
                        (*new_ch)[bit_idx + ch_striping_idx] = (*first)[bit_idx + ch_striping_idx];
                    }
                }
            }


            if(ch_bit_len_rem != 0) {
                /**
                 * Note: Warning condition occured in binary_striping_recombination_op:
                 * The striping size is not compatible with the chromosome size. This results in uneven striping.
                 */
                for(size_t bit_idx = ch_bit_len_striped; bit_idx < ch_bit_len_max; ++bit_idx) {
                    if(!side_first) {
                        (*new_ch)[bit_idx] = (*first)[bit_idx];
                    } else {
                        (*new_ch)[bit_idx] = (*second)[bit_idx];
                    }
                }
            }

            d->add(new_ch);
        }
        return d;
    }
};


namespace detail {


    template<size_t N, size_t Index = 0, typename Enable = void>
    struct n_point_recombination_op_apply {

        template<typename RangeArray>
        sp_hot void operator()(
                            const bool left_or_right,
                            const chromosome* first,
                            const chromosome* second,
                            chromosome* new_ch,
                            size_t last,
                            const size_t& size,
                            const RangeArray& array
                        ) {
            size_t len = Index < N ? array[Index] : size;
            for(size_t bit_idx = last; bit_idx < len; ++bit_idx) {
                if(left_or_right) {
                    (*new_ch)[bit_idx] = (*first)[bit_idx];
                } else {
                    (*new_ch)[bit_idx] = (*second)[bit_idx];
                }
            }
            n_point_recombination_op_apply<N, Index+1>()(!left_or_right, first, second, new_ch, len, size, array);
        }
    };

    template<size_t N, size_t Index>
    struct n_point_recombination_op_apply<N, Index, std::enable_if_t<Index == N+1>> {

        template<typename RangeArray>
        void operator()(    const bool left_or_right,
                            const chromosome* first,
                            const chromosome* second,
                            chromosome* new_ch,
                            size_t last,
                            size_t size,
                            const RangeArray& array) {}
    };

}
/**
 * \brief N Point Recombination
 *
 * Point based crossover
 * \param Striping_size the striping length
 */
template<size_t Points = 1>
struct n_point_recombination_op
    :   recombination_op<dna_ptr_pair, n_point_recombination_op<Points>> {

    static constexpr size_t points = Points;

    using operand_type = typename n_point_recombination_op::operand_type;
    using result_type = typename n_point_recombination_op::result_type;

    static_assert(points >= 1, "points must be greater than or equal to 1");

    template<typename Model>
    dna* impl(Model* m, population* p, const size_t& multi_count, dna_ptr_pair pair) {

        auto dna_len_1 = pair.first->size();
        auto dna_len_2 = pair.second->size();
        auto chromosome_count = std::max(dna_len_1, dna_len_2);

        dna* d = m->create_dna();
        d->reserve(chromosome_count);
        auto random = m->random();

        for(size_t new_chromosome_idx = 0; new_chromosome_idx < chromosome_count; ++new_chromosome_idx) {

            const chromosome* first = (*pair.first)[new_chromosome_idx];
            const chromosome* second = (*pair.second)[new_chromosome_idx];


            auto ch_bit_len_fir = first->size();
            auto ch_bit_len_sec = second->size();
            auto ch_bit_len_max = std::max(ch_bit_len_fir, ch_bit_len_sec);
            BOOST_ASSERT_MSG(ch_bit_len_fir == ch_bit_len_sec && ch_bit_len_sec == ch_bit_len_max, "Mismatching chromosome size not allowed");

            chromosome* new_ch = m->create_chromosome(ch_bit_len_max, false);

            std::array<size_t, points> indexes = sp::util::n_unique<points, size_t>(random, size_t(0), ch_bit_len_max);

            bool starting_side = random.rand_integral(0, 1);

            detail::n_point_recombination_op_apply<points>()(starting_side, first, second, new_ch, 0, ch_bit_len_max, indexes);

            d->add(new_ch);
        }
        return d;
    }
};

using single_point_recombination_op = n_point_recombination_op<1>;


/**
 * \brief Uniform Recombination
 *
 * Uniformily distributed genetic material from parent, i.e. each chromosome
 * bit has equal probabilty of being selected
 */
struct uniform_recombination_op
    :   recombination_op<dna_ptr_pair, uniform_recombination_op> {

    using operand_type = typename uniform_recombination_op::operand_type;
    using result_type = typename uniform_recombination_op::result_type;

    template<typename Model>
    dna* impl(Model* m, population* p, const size_t& multi_count, dna_ptr_pair pair) {

        auto dna_len_1 = pair.first->size();
        auto dna_len_2 = pair.second->size();
        auto chromosome_count = std::max(dna_len_1, dna_len_2);

        dna* d = m->create_dna();
        d->reserve(chromosome_count);
        auto random = m->random();

        for(size_t new_chromosome_idx = 0; new_chromosome_idx < chromosome_count; ++new_chromosome_idx) {

            const chromosome* first = (*pair.first)[new_chromosome_idx];
            const chromosome* second = (*pair.second)[new_chromosome_idx];

            auto ch_bit_len_fir = first->size();
            auto ch_bit_len_sec = second->size();
            auto ch_bit_len_max = std::max(ch_bit_len_fir, ch_bit_len_sec);

            chromosome* new_ch = m->create_chromosome(ch_bit_len_max, false);

            for(size_t bit_idx = 0; bit_idx < ch_bit_len_max; ++bit_idx) {
                if(random.rand_integral(0, 1)) {
                    (*new_ch)[bit_idx] = (*first)[bit_idx];
                } else {
                    (*new_ch)[bit_idx] = (*second)[bit_idx];
                }
            }

            d->add(new_ch);
        }
        return d;
    }

};


/***
 * Mutation strategy
 */
template<typename Derived>
struct mutation_op : multiplication_pipeline_op<dna*, dna*, Derived> {

    using operand_type = typename mutation_op::result_type;
    using result_type = typename mutation_op::result_type;

};

template<typename Derived>
struct mutation_rate {

    using derived_type = Derived;

    template<typename Model>
    float operator()(Model* m, dna* d) {
        return derived().impl(m, d);
    }

    /**
     * CRTP helper
     */
    derived_type const& derived() const {
        return *static_cast<derived_type const*>(this);
    }

};

/**
 * \brief Uniform mutation rate
 *
 * Uniform probability of
 *
 * Note: Currently the only mutation rate implementation
 */
template<std::intmax_t N, std::intmax_t D>
struct uniform_mutation_rate : mutation_rate<uniform_mutation_rate<N, D>> {

    static constexpr float mutation_rate = static_cast<float>(N) / static_cast<float>(D);
    static_assert(mutation_rate >= 0.0f && mutation_rate <= 1.0f, "Mutation rate must be in the range [0, 1]");

    template<typename Model>
    float operator()(Model* m, dna* d) {
        return mutation_rate;
    }

};

/***
 * \brief n bit mutation strategy based on probability for each bit
 * chromosome per DNA.
 *
 * \todo optimize, slow, N * 4 rand() function calls needed
 */
template<std::intmax_t N>
struct n_bit_based_mutation_op
    :    mutation_op<
            n_bit_based_mutation_op<
                N
            >
        > {

    static_assert(N >= 1, "N must be an greater or equal to 1");

    using result_type = typename n_bit_based_mutation_op::result_type;

    using operand_type = typename n_bit_based_mutation_op::operand_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type d) {
        for(std::intmax_t i = 0; i < N; ++i) {
            size_t ch_idx = d->size() * m->random().rand_float();
            chromosome* ch = (*d)[ch_idx];
            size_t  bits_size = ch->size(),
                    bit_idx = bits_size * m->random().rand_float();
            ch->flip(bit_idx);
        }
        return d;
    }
};

using single_bit_mutation_op = n_bit_based_mutation_op<1>;

/***
 * \brief Probability based mutation strategy
 *
 * Considers all bits equal based on a Mutation_rate_type specified
 *
 * \todo optimize. It is slow because 1 rand() func call is needed
 * per bit per chromosome per DNA
 */
template<typename MutationRate>
struct probability_based_mutation_strategy
    :    mutation_op<
            probability_based_mutation_strategy<
                MutationRate
            >
        > {

    using result_type = typename probability_based_mutation_strategy::result_type;

    using operand_type = typename probability_based_mutation_strategy::operand_type;

    template<typename Model>
    result_type impl(Model* m, population* p, const size_t& multi_count, operand_type d) {
        size_t cl = d->size();
        for(size_t ci = 0; ci < cl; ++ci) {
            chromosome* ch = (*d)[ci];
            for(size_t i = 0, l = ch->size(); i < l; ++i) {
                if(MutationRate()(m, d) >= m->random().rand_float()) {
                    size_t bit_idx = ch->size() * m->random().rand_float();
                    ch->flip(bit_idx);
                }
            }
        }
        return d;
    }
};

/**
 * \brief Migration Model manages the migration between the different populations
 * and islands
 *
 *
 */
template<typename Derived>
struct migration_model {

    using derived_type = Derived;

    template<typename Model>
    void operator()(Model* m, population* p) {
        return derived().impl(m, p);
    }

    /**
     * CRTP helper
     */
    derived_type & derived() {
        return *reinterpret_cast<derived_type*>(this);
    }

};

/**
 * \brief Zero Migration Model
 */
struct zero_migration_model : migration_model<zero_migration_model> {

    template<typename Model>
    void impl(Model* m, population* p) {}

};


/**
 * \brief Meiosis multiplication strategy
 *
 * Meiosis strategy replicates genetic material, which then divide into two more
 * daughter cells, producing 4 daughter cells with half the number of genetic
 * material of the parent cell
 *
 */
template<std::intmax_t N = 1, std::intmax_t D = 100>
using meiosis_strategy =
    multiplication_pipeline_strategy<
        fitness_proportionate_binary_selection_op,
        n_point_recombination_op<1>,
        probability_based_mutation_strategy<
            uniform_mutation_rate<N, D>
        >
    >;

/**
 * \brief Mitosis multiplication strategy
 */
template<std::intmax_t N_bit_ops = 2, std::intmax_t N = 1, std::intmax_t D = 100>
using mitosis_strategy = multiplication_pipeline_strategy<
    fitness_proportionate_unary_selection_op,
    simulated_binary_fission,
    probability_based_mutation_strategy<
        uniform_mutation_rate<N, D>
    >
>;

/**
 * \brief Simple Meiosis based Evolutionary Programming Model
 */
template<typename FitnessEvaluator>
using simple_meiosis_model = model<
    meiosis_strategy<>,
    ratio_population_control<
        std::ratio<0>,
        std::ratio<25,100>,
        FitnessEvaluator
    >,
    zero_migration_model
>;

/**
 * \brief Simple Mitosis based Evolutionary Programming Model
 */
template<typename FitnessEvaluator>
using simple_mitosis_model = model<
    mitosis_strategy<>,
    ratio_population_control<
        std::ratio<0>,
        std::ratio<25,100>,
        FitnessEvaluator
    >,
    zero_migration_model
>;

/**
 * \brief Zero Model
 *
 * Simply put a no-op model
 */
using zero_model = model<
        multiplication_pipeline_strategy<
            zero_selection_op,
            zero_recombination_op
        >,
        zero_population_control<zero_fitness_evaluator>,
        zero_migration_model
>;

/**
 * \brief Bit Reader base class
 *
 * The intention is to serve as a way of incrementally way of reading
 * a source of binary data, constructing various data types from the values it
 * reads. The bit_reader must be constructed with default parameter N and Operand_type
 *
 * \tparam N the default number of bits to read
 * \tparam T the default type to convert to
 *
 */
template<typename Derived>
struct bit_reader {

    using derived_type = Derived;

    /**
     * \brief Returns true if there is still data to be read of at least
     * Bits
     */
    template<size_t Bits>
    bool has_next() {
        return derived().template has_next_impl<Bits>();
    }

    /**
     * \brief Returns true if there is still data to be read of at least
     * Bits
     */
    template<size_t Bits >
    bool has_next() const {
        return derived().template has_next_impl<Bits>();
    }
\
    /**
     * \brief Returns true if there is still at least some data, partial or not
     * to be read
     */
    bool has_next_partial() {
        return derived().has_next_partial_impl();
    }

    /**
     * \brief Returns true if there is still at least some data, partial or not
     * to be read
     */
    bool has_next_partial() const {
        return derived().has_next_partial_impl();
    }

    /**
     * Reset the reader
     */
    void reset() {
        seek(0);
    }

    /**
     * Set the current reading position to the specified position
     */
    void seek(size_t pos) {
        derived().seek_impl(pos);
    }

    size_t pos() {
        return derived().pos_impl();
    }

    size_t pos() const {
        return derived().pos_impl();
    }

    void advance(size_t val) {
        seek(pos()+val);
    }

    /**
     * \brief Retrieve the next value
     *
     * \remark It is a programming error to call this function
     * if has_next() is false
     */
    template<size_t Bits, typename Result>
    Result next() {
        static_assert(sizeof(Result) * CHAR_BIT >= Bits, "Size of Result_type must be greater or equal to Bits");
        return derived().template next_impl<Bits, Result>();
    }

    /**
     * \brief Retrieves the next value without incrementing
     * the internal counter
     */
    template<size_t Bits, typename Result>
    Result peek() {
        auto p = pos();
        auto res = next<Bits, Result>();
        seek(p);
        return res;
    }

    derived_type& derived() {
        return *static_cast<derived_type*>(this);
    }
};

/**
 * \brief Chromosome Bit to Integral Reader
 *
 * Reads N bits at a time and converts it to type T. The read bits are filled in
 * from left to right, for example: a 8 bit chromosome consisting of the bits
 * 11111110 will be read as a -1.
 *
 * \remark Beware of signed number encoding as they can produce unexpected
 * results when the sign bit is set. Furthermore, if chromosome is not evenly
 * divisible by N, then if the the missing bits will default to 0. This means
 * that if only one remaining bit is left and is true and type T is unsigned,
 * it'll be read as an negative value (i.e. std::numeric_limits<T>::min()) as
 * only the sign bit was flipped.
 *
 * \todo consider edge cases
 */
struct chromosome_bit_to_integral_reader
    : bit_reader<chromosome_bit_to_integral_reader> {

    chromosome_bit_to_integral_reader(const chromosome* const ch)
        : chrom(ch), idx(), size(ch->size()) {}

    template<size_t Bits>
    bool has_next_impl() {
        return (idx + Bits) <= size;
    }

    template<size_t Bits>
    bool has_next_impl() const {
        return (idx + Bits) <= size;
    }

    bool has_next_partial_impl() {
        return idx < size;
    }

    bool has_next_partial_impl() const {
        return idx < size;
    }

    void seek_impl(size_t pos) {
        idx = pos;
    }

    size_t pos_impl() {
        return idx;
    }

    size_t pos_impl() const {
        return idx;
    }

    /**
     * \todo Optimize
     */
    template<size_t Bits, typename Result>
    Result next_impl() {
        static_assert(std::is_integral<Result>::value || std::is_same<Result, bool>::value, "Type is neither integral nor boolean");
        static_assert(!(std::is_same<Result, bool>::value && Bits != 1), "it is an error to read more than 1 bit into a bool");
        //workaround for bool, size_of
        using calc_type = std::conditional_t<std::is_same<Result, bool>::value, std::uint_fast8_t, Result>;
        constexpr size_t read_bits = std::is_same<Result, bool>::value ? 1 : Bits;
        calc_type result = 0;
        for(int
                i = read_bits-1,
                min = std::max<int>(read_bits- (size-idx), 0);
                i >= min;
                --i) {
            calc_type data = (*chrom)[idx];
            result |= (data << i);
            ++idx;
        }
        return result;
    }

private:
    const chromosome* const chrom;
    size_t idx;
    size_t size;
};


/**
 * \brief Genetic Expression base class
 *
 * An actionable genetic expression that is the result of the genetic_expression_generator
 *
 *see Should a genetic_expression_generator produce a functor  actionable 
 */
template<typename Derived, typename Result, typename ... Arguments>
struct genetic_expression {

    using result_type = Result;
    using derived_type = Derived;

    result_type operator()(Arguments ... args) {
        return derived().impl(std::forward<Arguments>(args)...);
    }

    derived_type& derived() {
        return *static_cast<derived_type*>(this);
    }
};

/**
 * \brief Genetic Expression Generator super type
 *
 * The base type of all possible genetic expressions. It takes the address
 * of a Chromosome and returns the produced expression. The process of which is implementation
 * defined
 * 
 * The result can be of any type. However, see genetic_expression for actionable
 * results
 */
template<
    typename Result,
    typename Derived,
    typename Reader
>
struct genetic_expression_generator {

    using result_type = Result;
    using derived_type = Derived;
    using reader_type = Reader;

    result_type operator()(const chromosome* const ch) const {
        return derived().impl(ch);
    }

    derived_type& derived() {
        return *static_cast<derived_type*>(this);
    }
};


}}}


#endif	/* SP_ALGO_GEN_HPP */

