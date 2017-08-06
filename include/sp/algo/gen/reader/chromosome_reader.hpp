/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_READER_READER_CHROMOSOME_READER_HPP
#define SP_ALGO_GEN_READER_READER_CHROMOSOME_READER_HPP

#include "reader.hpp"

SP_ALGO_GEN_NAMESPACE_BEGIN

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
struct chromosome_bit_reader
    : bit_reader<chromosome_bit_reader> {

    chromosome_bit_reader(const chromosome* const ch)
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

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_READER_READER_CHROMOSOME_READER_HPP */

