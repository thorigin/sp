/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#include <iostream>
#include "sp/algo/stats.hpp"
#include "sp/algo/gen.hpp"
#include "sp/util/units.hpp"

#include "sp/util/timing.hpp"
#include "sp/util/query.hpp"
#include "sp/util/typename.hpp"
#define BOOST_TEST_MODULE sp_algo_gen
#include <boost/test/unit_test.hpp>

using namespace sp::algo::gen;
using namespace sp::algo;
using namespace sp::util::units;
using namespace sp::util;


template<typename Selection_op_type>
struct binary_selection_op_fixture {

    binary_selection_op_fixture() : m(), select_op(), freq_map() {}

    ~binary_selection_op_fixture() {

    }

    population_ptr create_demo_pop(std::initializer_list<float> evaluations) {
        auto pop = m.new_island()->new_pop();
        m.seed(evaluations.size(), {}, pop);
        size_t idx = 0;
        for(auto    it = evaluations.begin(),
                    end = evaluations.end();
                    it != end;
                    ++it) {
            (*pop)[idx]->set_eval(*it);
            ++idx;
        }
        return pop;
    }

    void collect_samples(const population_ptr& pop, size_t samples) {
        for(size_t i = 0; i < samples; ++i) {
            auto res = select_op(&m, pop.get(), -1, 0);
            ++freq_map[res.first];
        }
    }

    simple_meiosis_model<zero_fitness_evaluator> m;
    Selection_op_type select_op;
    std::map<dna*, size_t> freq_map;
};

BOOST_AUTO_TEST_CASE( chromosome_operations) {
    chromosome ch(4);
    ch[0] = 0;
    ch[1] = 1;
    ch[2] = 0;
    ch[3] = 1;
    BOOST_REQUIRE_EQUAL(ch[0], 0);
    BOOST_REQUIRE_EQUAL(ch[1], 1);
    BOOST_REQUIRE_EQUAL(ch[2], 0);
    BOOST_REQUIRE_EQUAL(ch[3], 1);
    ch.flip(3);
    BOOST_REQUIRE_EQUAL(ch[3], 0);
    ch.set_value(2, 1);
    BOOST_REQUIRE_EQUAL(ch[2], 1);
}

BOOST_AUTO_TEST_CASE( dna_operations) {
    dna d;
    chromosome* ch = new chromosome(1);
    d.add(ch);
    BOOST_REQUIRE_EQUAL(d[0], ch);
    BOOST_REQUIRE_EQUAL(d[0]->size(), ch->size());
    BOOST_REQUIRE_EQUAL(d.size(), 1);
}

BOOST_AUTO_TEST_CASE( population_operations ) {
    population p;
    dna* d = new dna();
    BOOST_REQUIRE_EQUAL(p.size(), 0);
    p.add(d);
    BOOST_REQUIRE_EQUAL(p.size(), 1);
    BOOST_REQUIRE_EQUAL(p.evolutions(), 0);
    p.incr_evolutions();
    BOOST_REQUIRE_EQUAL(p.evolutions(), 1);
}
BOOST_AUTO_TEST_CASE( population_remove_from_bottom_should_remove_bottom_entries ) {
    population p;
    dna* d = new dna();
    dna* d2 = new dna();
    dna* d3 = new dna();
    p.add(d);
    p.add(d2);
    p.add(d3);
    BOOST_REQUIRE_EQUAL(p.size(), 3);
    p.remove_from_bottom(1);
    BOOST_REQUIRE_EQUAL(p.size(), 2);
    BOOST_REQUIRE_EQUAL(p[0], d);
    BOOST_REQUIRE_EQUAL(p[1], d2);
}

BOOST_AUTO_TEST_CASE( query_container_limit_1_shoud_return_first_in_list ) {
    island isl;
    auto p = isl.new_pop();
    dna* d1 = new dna();
    dna* d2 = new dna();
    p->add(d1);
    p->add(d2);

    auto res = query(p->begin(), p->end(), sp_query_criteria(d, true), highest_dna_eval_comparator(), limit(1));
    BOOST_REQUIRE_EQUAL(res.size(), 1);
    BOOST_REQUIRE_EQUAL(res[0], d1);
    BOOST_REQUIRE_EQUAL((*p)[0], res[0]);
}

BOOST_AUTO_TEST_CASE( query_container_with_sorting_and_limit_1_should_return_reverse_in_list ) {
    island isl;
    auto p = isl.new_pop();
    dna* d1 = new dna();
    d1->set_eval(0.0f);
    dna* d2 = new dna();
    d2->set_eval(1.0f);
    p->add(d1);
    p->add(d2);

    auto res = query(
        p->begin(), p->end(),
        sp_query_criteria(d, true),
        sp_query_cmpr(l, r, l->eval() < r->eval()),
//        [](const dna* const d) { return true; },
//        [](const dna* const lhs, const dna* const rhs) { return lhs->eval() > rhs->eval(); },
        limit(1)
    );
    BOOST_REQUIRE_EQUAL(res.size(), 1);
    BOOST_REQUIRE_EQUAL(res[0], d1);
    BOOST_REQUIRE_EQUAL((*p)[0], res[0]);

}
BOOST_AUTO_TEST_CASE( island_new_pop_should_return_shared_ptr_and_add_to_self ) {
    island isl;
    auto p = isl.new_pop();
    BOOST_REQUIRE_EQUAL(isl.size(), 1);
    auto p2 = isl.new_pop();
    BOOST_REQUIRE_EQUAL(isl.size(), 2);
    BOOST_REQUIRE_EQUAL(isl[0], p);
    BOOST_REQUIRE_EQUAL(isl[1], p2);
}

BOOST_AUTO_TEST_CASE( default_population_evaluator_should_assign_evaluation_to_all_and_sort_the_result ) {

    default_population_evaluator<zero_fitness_evaluator> dpe;
    zero_model m;
    auto isl = m.new_island();
    auto p = isl->new_pop();
    dna* d1 = new dna(); d1->set_eval(1.0f);
    dna* d2 = new dna(); d2->set_eval(1.0f);
    dna* d3 = new dna(); d3->set_eval(1.0f);
    dna* d4 = new dna(); d4->set_eval(1.0f);
    p->add(d1);
    p->add(d2);
    p->add(d3);
    p->add(d4);

    dpe(&m, p.get(), 0, p->size());

    BOOST_REQUIRE_EQUAL((*p)[0]->eval(), 0.0f);
    BOOST_REQUIRE_EQUAL((*p)[1]->eval(), 0.0f);
    BOOST_REQUIRE_EQUAL((*p)[2]->eval(), 0.0f);
    BOOST_REQUIRE_EQUAL((*p)[3]->eval(), 0.0f);
}

BOOST_AUTO_TEST_CASE( perc_population_control_should_return_percent_of_population ) {

    ratio_population_control<std::ratio<1,2>, std::ratio<1,2>, zero_fitness_evaluator> ppc;
    zero_model m;
    auto isl = m.new_island();
    auto p = isl->new_pop();
    dna* d1 = new dna(); d1->set_eval(1.0f);
    dna* d2 = new dna(); d2->set_eval(1.0f);
    dna* d3 = new dna(); d3->set_eval(1.0f);
    dna* d4 = new dna(); d4->set_eval(1.0f);
    p->add(d1);
    p->add(d2);
    p->add(d3);
    p->add(d4);

    BOOST_REQUIRE_EQUAL(ppc.add_count(&m, p.get()), 4);
    BOOST_REQUIRE_EQUAL(ppc.prune_count(&m, p.get()), 2);

}


BOOST_AUTO_TEST_CASE( binary_striping_recombination_op_with_striping_of_2_should_interleave_two_chromosomes) {

    striping_recombination_binary_op<2> bsro;
    zero_model m;
    auto pop = m.new_island()->new_pop();
    dna* d1 = new dna();
    dna* d2 = new dna();
    chromosome* ch1 = new chromosome(4);
    (*ch1)[0] = 1; (*ch1)[1] = 1; (*ch1)[2] = 1; (*ch1)[3] = 1;
    chromosome* ch2 = new chromosome(4);
    (*ch2)[0] = 0; (*ch2)[1] = 0; (*ch2)[2] = 0; (*ch2)[3] = 0;
    d1->add(ch1);
    d2->add(ch2);
    pop->add(d1);
    pop->add(d2);
    typename striping_recombination_binary_op<>::operand_type pair((*pop)[0], (*pop)[1]);
    dna* result = bsro(&m, nullptr, 0, pair);

    chromosome& p1 = *ch1,
                p2 = *ch2,
                ch = *(*result)[0];

    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(result->size(), (*pop)[0]->size());
    BOOST_REQUIRE_EQUAL(result->size(), (*pop)[1]->size());

    BOOST_REQUIRE_EQUAL(ch.size(), p1.size());
    BOOST_REQUIRE_EQUAL(ch.size(), p2.size());

    BOOST_REQUIRE_EQUAL(ch.size(), p1.size());
    BOOST_REQUIRE_EQUAL(ch.size(), p2.size());

    BOOST_REQUIRE(ch[0] == p1[0] || ch[0] == p2[0]);
    BOOST_REQUIRE(ch[1] == p1[1] || ch[1] == p2[1]);
    BOOST_REQUIRE(ch[2] == p2[2] || ch[2] == p1[2]);
    BOOST_REQUIRE(ch[0] == p2[3] || ch[3] == p1[3]);
    
}

BOOST_FIXTURE_TEST_CASE(    fitness_propotionate_selection_op_selection_frequency_matches_evaluation,
                            binary_selection_op_fixture<fitness_proportionate_binary_selection_op> ) {
    auto p = create_demo_pop({0.1, 0.5, 0.9, 1.0});
    auto total = 0.1 + 0.5 + 0.9 + 1.0;
    size_t samples = 10000;
    collect_samples(p, samples);
    for(auto& s : freq_map) {
        auto expected = s.first->eval() / total * samples;
        auto tolerance = 15; //% tolerance
        BOOST_REQUIRE_CLOSE(expected, s.second, tolerance);
        std::cout << s.first->eval() << " = " << s.second << std::endl;
    }
    //BOOST_REQUIRE((*p)[0] > 100);
}

BOOST_AUTO_TEST_CASE( simple_meiosis_model_evolve_replaces_at_least_one ) {

    simple_meiosis_model<zero_fitness_evaluator> m;

    auto pop = m.new_island()->new_pop();
    m.seed(2, {1}, pop);
    BOOST_REQUIRE_EQUAL(pop->size(), 2);
    m.evolve(pop, sp_evolve_until(ctx.evol_count() >= 1));
    BOOST_REQUIRE_EQUAL(pop->size(), 2);
    dna* first2 = (*pop)[0];
    dna* second2 = (*pop)[1];
    //One of them must have been changed
    BOOST_REQUIRE(first2->lifetime() == 0 || second2->lifetime() == 0);

}

BOOST_AUTO_TEST_CASE(   int_simple_meiosis_binary_string_bit_count_test , 
                        *boost::unit_test::disabled() *boost::unit_test::label("integration")) {

    //Adds up the number of bits (ratio)
    struct binary_evaluator {
        float operator()(const dna* const d) const {
            size_t count = 0;
            size_t total = 0;
            for(size_t ch_idx = 0, ch_len = d->size(); ch_idx < ch_len; ++ch_idx) {
                const chromosome* ch = (*d)[ch_idx];
                for(size_t bit_idx = 0, bit_len = ch->size(); bit_idx < bit_len; ++bit_idx) {
                    count += (*ch)[bit_idx];
                }
                total += ch->size();
            }
            return static_cast<float>(count) / static_cast<float>(total);
        }
    };

    simple_meiosis_model<binary_evaluator> m;

    auto p = m.new_island()->new_pop();

    m.seed(units::kilo(32), {units::kilo(512)}, p);

    scoped_timer timer;
    std::cout << "Memory Usage: " << stringifyBytes(p->mem_size()) << std::endl;

    m.evolve(p,
        sp_evolve_until(ctx.highest_eval() >= 0.999f)
    );

    std::cout << "DNAs processed per second: " << timer.per_second(p->evolutions()*p->size()) << "\n";
    std::cout << "Population evolved over " << p->evolutions() << " generations\n";
    std::cout << "Memory Usage: " << stringifyBytes(p->mem_size()) << std::endl;
//    std::cout << make_stats("Evaluation", *p, [](const dna* const d) { return d->eval(); });
//    std::cout << make_stats("Lifetime", *p, [](const dna* const d) { return d->lifetime(); });

}

BOOST_AUTO_TEST_CASE( chromosome_bit_to_integral_reader_reads_signs_correctly_with_regard_to_type) {

    using reader_type = chromosome_bit_reader;
    chromosome ch{1,0,0,0,0,0,0,1,1};
    reader_type cbtir(&ch);

    using tp = signed char;
    constexpr size_t N = 8;
    BOOST_REQUIRE(cbtir.has_next<N>());
    BOOST_REQUIRE_EQUAL(((int)cbtir.peek<N, tp>()), -127);
    BOOST_REQUIRE_EQUAL(((int)cbtir.next<N, tp>()), -127);
    ch[0] = 0;
    cbtir.seek(0);
    BOOST_REQUIRE_EQUAL(((int)cbtir.peek<N, tp>()), 1);
    BOOST_REQUIRE_EQUAL(((int)cbtir.next<N, tp>()), 1);
    ch[6] = 1;
    cbtir.seek(0);
    BOOST_REQUIRE_EQUAL(((int)cbtir.peek<N, tp>()), 3);
    BOOST_REQUIRE_EQUAL(((int)cbtir.next<N, tp>()), 3);
    BOOST_REQUIRE((cbtir.has_next_partial()));
    BOOST_REQUIRE_EQUAL(((int)cbtir.peek<N, tp>()), -128);
    BOOST_REQUIRE_EQUAL(((int)cbtir.next<N, tp>()), -128);
    BOOST_REQUIRE((!cbtir.has_next<N>()));
}
