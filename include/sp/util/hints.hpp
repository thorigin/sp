/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_UTIL_HINTS_HPP
#define	SP_UTIL_HINTS_HPP


/**
 * \internal Optimizations hints
 */
#ifdef __GNUC__
#define sp_restrict __restrict__
/**
 * hot, inline
 */
#define sp_hot inline __attribute__((hot, always_inline))
/**
 * hot, pure, inline
 */
#define sp_hot_pure inline __attribute__((hot, pure, always_inline))
/**
 * hot, noinline
 */
#define sp_hot_no_inline __attribute__((hot, noinline))
/**
 * hot, pure, no inline
 */
#define sp_no_inline __attribute__((hot, noinline))

#define sp_likely(x)       __builtin_expect(!!(x), 1)

#define sp_unlikely(x)     __builtin_expect(!!(x), 0)

#else
#define sp_restrict
#define sp_hot inline
#define sp_hot_pure inline
#define sp_hot_no_inline
#define sp_no_inline
#define likely(x) (x)
#define unlikely(x) (x)
#endif


#endif /* SP_UTIL_HINTS_HPP */

