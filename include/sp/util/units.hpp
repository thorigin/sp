/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_UTIL_UNITS_HPP
#define	SP_UTIL_UNITS_HPP

#include <sstream>
#include <string>
#include <iomanip>

SP_UTIL_NAMESPACE_BEGIN
namespace units {

template<typename T>
constexpr inline T trillion(T value) {
    return value * 1000 * 1000 * 1000 * 1000;
}
template<typename T>
constexpr inline T billion(T value) {
    return value * 1000 * 1000 * 1000;
}

template<typename T>
constexpr inline T million(T value) {
    return value * 1000 * 1000;
}

template<typename T>
constexpr inline T thousand(T value) {
    return value * 1000;
}

template<typename T>
constexpr inline T tera(T value) {
    return value * 1024 * 1024 * 1024 * 1024;
}

template<typename T>
constexpr inline T giga(T value) {
    return value * 1024 * 1024 * 1024;
}

template<typename T>
constexpr inline T mega(T value) {
    return value * 1024 * 1024;
}

template<typename T>
constexpr inline T kilo(T value) {
    return value * 1024;
}

/**
 * \brief converts number of bytes into a human readable form
 * of the largest unit
 *
 * \param bytes
 * \return
 */
template<size_t Precision = 2, typename T>
inline std::string stringifyBytes(T bytes) {

    float tb = tera<float>(1);
    float gb = giga<float>(1);
    float mb = mega<float>(1);
    float kb = kilo<float>(1);

    std::stringstream buffer;
    buffer << std::fixed << std::setprecision(Precision);

    if(bytes >= tb) {
        buffer << (bytes/ tb) <<  " tB";
    } else if(bytes >= gb) {
        buffer << (bytes/ gb) <<  " gB";
    } else if(bytes >= mb) {
        buffer << (bytes/ mb) <<  " mB";
    } else if(bytes >= kb) {
        buffer << (bytes/ kb) <<  " kB";
    } else {
        buffer << (bytes) <<  " B";
    }

    return buffer.str();
}

}
SP_UTIL_NAMESPACE_END

#endif /* SP_UTIL_UNITS_HPP */