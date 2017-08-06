/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATION_N_POINT_RECOMBINATION_BINARY_OP_HPP
#define	SP_ALGO_GEN_MULTIPLICATION_N_POINT_RECOMBINATION_BINARY_OP_HPP


SP_ALGO_GEN_NAMESPACE_BEGIN

namespace detail {

template<size_t N, size_t Index = 0, typename Enable = void>
struct n_point_recombination_binary_op_apply {

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
        n_point_recombination_binary_op_apply<N, Index+1>()(!left_or_right, first, second, new_ch, len, size, array);
    }
};

template<size_t N, size_t Index>
struct n_point_recombination_binary_op_apply<N, Index, std::enable_if_t<Index == N+1>> {

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
struct n_point_recombination_binary_op
    : recombination_op<dna_ptr_pair, n_point_recombination_binary_op<Points>> {

    static constexpr size_t points = Points;

    using operand_type = typename n_point_recombination_binary_op::operand_type;
    using result_type = typename n_point_recombination_binary_op::result_type;

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

            detail::n_point_recombination_binary_op_apply<points>()(starting_side, first, second, new_ch, 0, ch_bit_len_max, indexes);

            d->add(new_ch);
        }
        return d;
    }
};

/**
 * \brief Signle Point Recombination Op
 */
using single_point_recombination_binary_op = n_point_recombination_binary_op<1>;

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATION_N_POINT_RECOMBINATION_BINARY_OP_HPP */

