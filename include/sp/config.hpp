/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_CONFIG_HPP
#define SP_CONFIG_HPP

#define SP_NAMESPACE_BEGIN  namespace sp {
#define SP_NAMESPACE_END    }

#define SP_ALGO_NAMESPACE_BEGIN namespace algo {
#define SP_ALGO_NAMESPACE_END   }

#define SP_ALGO_GEN_NAMESPACE_BEGIN SP_NAMESPACE_BEGIN SP_ALGO_NAMESPACE_BEGIN namespace gen {
#define SP_ALGO_GEN_NAMESPACE_END   } SP_ALGO_NAMESPACE_END SP_NAMESPACE_END

#define SP_UTIL_NAMESPACE_BEGIN     SP_NAMESPACE_BEGIN namespace util {
#define SP_UTIL_NAMESPACE_END       } SP_NAMESPACE_END

#define SP_ALGO_GEN_PARSER_NAMESPACE_BEGIN   SP_ALGO_GEN_NAMESPACE_BEGIN namespace parser {
#define SP_ALGO_GEN_PARSER_NAMESPACE_END     } SP_ALGO_GEN_NAMESPACE_END

#endif /* SP_CONFIG_HPP */

