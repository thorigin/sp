/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MODEL_MODEL_HPP
#define	SP_ALGO_GEN_MODEL_MODEL_HPP

#include <vector>
#include <utility>
#include <memory>
#include <boost/random.hpp>
#include "sp/config.hpp"
#include "sp/util/rand.hpp"
#include "stop_context.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

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
        : mult_strategy(),
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
        stop_context stop_ctx(p);
        do {
            size_t  add_ct = pop_ctrl_strategy.add_count(this, p),
                    prune_ct = pop_ctrl_strategy.prune_count(this, p);


            pop_ctrl_strategy.evaluate(this, p, p->size()-prune_ct, p->size());

//            if(p->evolutions() % 1000 == 0) {
//                std::cout << sp::algo::make_stats("Evolutions", *p, [](const dna* const d) { return d->eval(); });
//            }

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


SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MODEL_MODEL_HPP */

