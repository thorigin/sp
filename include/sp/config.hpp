/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_CONFIG_HPP
#define SP_CONFIG_HPP

/**
 * \file Contains configuration and macro definitions used in this project
 */


#if !(   __GNUC__ == 7 && __GNUC_MINOR__ == 3)
#warning "This software has only been tested on GCC 7.3"
#endif



#define SP_NAMESPACE_BEGIN  namespace sp {
#define SP_NAMESPACE_END    }

#define SP_TESTING_NAMESPACE_BEGIN  SP_NAMESPACE_BEGIN namespace testing {
#define SP_TESTING_NAMESPACE_END    } SP_NAMESPACE_END

#define SP_ALGO_NAMESPACE_BEGIN SP_NAMESPACE_BEGIN namespace algo {
#define SP_ALGO_NAMESPACE_END   } SP_NAMESPACE_END

#define SP_ALGO_MATH_NAMESPACE_BEGIN SP_ALGO_NAMESPACE_BEGIN namespace math {
#define SP_ALGO_MATH_NAMESPACE_END   } SP_ALGO_NAMESPACE_END

#define SP_ALGO_GEN_NAMESPACE_BEGIN SP_ALGO_NAMESPACE_BEGIN namespace gen {
#define SP_ALGO_GEN_NAMESPACE_END   } SP_ALGO_NAMESPACE_END

#define SP_ALGO_NN_NAMESPACE_BEGIN SP_ALGO_NAMESPACE_BEGIN namespace nn {
#define SP_ALGO_NN_NAMESPACE_END   } SP_ALGO_NAMESPACE_END

#define SP_ALGO_NN_DETAIL_NAMESPACE_BEGIN SP_ALGO_NN_NAMESPACE_BEGIN namespace detail {
#define SP_ALGO_NN_DETAIL_NAMESPACE_END   } SP_ALGO_NN_NAMESPACE_END

#define SP_UTIL_NAMESPACE_BEGIN     SP_NAMESPACE_BEGIN namespace util {
#define SP_UTIL_NAMESPACE_END       } SP_NAMESPACE_END

#define SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN   SP_ALGO_GEN_NAMESPACE_BEGIN namespace parser {
#define SP_ALGO_GEN_PARSER_NAMESPACE_END     } SP_ALGO_GEN_NAMESPACE_END

#endif /* SP_CONFIG_HPP */

