/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATION_STRIPING_RECOMBINATION_BINARY_OP_HPP
#define	SP_ALGO_GEN_MULTIPLICATION_STRIPING_RECOMBINATION_BINARY_OP_HPP

#include "pipeline_op.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

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
struct striping_recombination_binary_op
    : recombination_op<dna_ptr_pair, striping_recombination_binary_op<StripingSize>> {

    static constexpr std::intmax_t striping_size = StripingSize;
    static constexpr std::intmax_t double_striping_size = striping_size * 2;


    using operand_type = typename striping_recombination_binary_op::operand_type;
    using result_type = typename striping_recombination_binary_op::result_type;

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

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATION_STRIPING_RECOMBINATION_BINARY_OP_HPP */

