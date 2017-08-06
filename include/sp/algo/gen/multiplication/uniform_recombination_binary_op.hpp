/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATION_UNIFORM_RECOMBINATION_BINARY_OP_HPP
#define	SP_ALGO_GEN_MULTIPLICATION_UNIFORM_RECOMBINATION_BINARY_OP_HPP


SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Uniform Recombination Binary Op
 *
 * Uniformily distributed genetic material from parent, i.e. each chromosome
 * bit has equal probabilty of being selected
 */
struct uniform_recombination_binary_op
    : recombination_op<dna_ptr_pair, uniform_recombination_binary_op> {

    using operand_type = typename uniform_recombination_binary_op::operand_type;
    using result_type = typename uniform_recombination_binary_op::result_type;

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


SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATION_UNIFORM_RECOMBINATION_BINARY_OP_HPP */

