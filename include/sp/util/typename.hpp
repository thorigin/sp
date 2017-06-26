/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_UTIL_TYPENAME_HPP
#define	SP_UTIL_TYPENAME_HPP

#include <sstream>
#include <string>
#include <iomanip>
#include <cstddef>

#ifdef __GNUG__
#include <cxxabi.h>
#endif
namespace sp { namespace util {

/**
 * \brief Helper function, attempts to demangles ABI name to human readable type name and
 * returns the result as a std::string
 */
template<typename T>
std::string type_name()
{
#ifdef __GNUG__
    int status;
    std::string tname = typeid(T).name();
    char *demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
    if(status == 0) {
        tname = demangled_name;
        std::free(demangled_name);
    }

    return tname;
#else
    return typeid(T).name();
#endif
}


}}

#endif /* SP_UTSP_UTIL_TYPENAME_HPPIL_UNITS_HPP */