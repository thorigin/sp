/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_READER_READER_HPP
#define SP_ALGO_GEN_READER_READER_HPP

SP_ALGO_GEN_NAMESPACE_BEGIN

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

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_READER_READER_HPP */

