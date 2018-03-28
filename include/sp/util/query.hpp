/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_UTIL_QUERY_HPP
#define SP_UTIL_QUERY_HPP

#include <vector>
#include <utility>
#include <algorithm>

SP_UTIL_NAMESPACE_BEGIN

/**
 * \brief type alias for size_t
 */
using limit = size_t;

/**
 * \brief Query a range by the given critiera expression and sorting order
 *
 *
 * \todo improve efficiency and memory usage
 *
 * \param cont the container to query
 * \param criteria the criteria benqto query by (see sp_query_criteria)
 * \param lim the limit of results
 * \return the result
 */
template<typename Iterator, typename Critieria>
auto query(
            Iterator&& first,
            Iterator&& last,
            Critieria&& criteria,
            limit&& lim = std::numeric_limits<limit>::max()) {
    return query(
        std::forward<Iterator>(first),
        std::forward<Iterator>(last),
        std::forward<Critieria>(criteria),
        std::less<>(),
        std::forward<limit>(lim)
    );
}
/**
 * \brief Query a range by the given critiera expression and sorting order
 *
 *
 * \todo improve efficiency and memory usage, multi key sorting
 *
 * \param cont the container to query
 * \param criteria the criteria benqto query by (see sp_query_criteria)
 * \param comparator the sorting comparator (see sp_query_cmpr)
 * \param lim the limit of results
 * \return the result
 */
template<typename Iterator, typename Critieria, typename Comparator>
auto query(
            Iterator&& first,
            Iterator&& last,
            Critieria&& criteria,
            Comparator&& comparator,
            limit&& lim = std::numeric_limits<limit>::max()) {
    std::vector<typename std::iterator_traits<Iterator>::value_type> res(first, last);
    std::sort(res.begin(), res.end()-1, std::forward<Comparator>(comparator));
    res.erase(
        std::remove_if(
            res.begin(),
            res.end(),
            [&](const auto& val) { return !criteria(val); }
        ),
        res.end()
    );
    //limit
    auto size = res.size();
    res.erase(res.begin() + std::min(size, lim), res.end());
    return res;
}

/**
 * \brief Macro helper function to create a search criteria
 *
 * \param key the variable name of the entity being searched
 * \param expression the boolean expression using the specified key
 */
#define sp_query_criteria(key, expression) [](const auto& (key)) { return (expression); }

/**
 * \brief Macro helper function to create simple sorting criteria
 *
 * \param left
 * \param right
 * \param expression the sorting expression to sort the container
 */
#define sp_query_cmpr(left, right, expression) [](const auto* left, const auto* right) { return (expression); }


SP_UTIL_NAMESPACE_END


#endif /* SP_UTIL_QUERY_HPP */