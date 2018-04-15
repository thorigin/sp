/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#define BOOST_TEST_MODULE sp_util_lookback
#include <boost/test/unit_test.hpp>
#include <string>

#include <iostream>

#include "sp/util/lookback.hpp"

using namespace sp::util;

BOOST_AUTO_TEST_CASE( test_sp_util_lookback_simple) {

    fixed_lookback<float> hist(20);


    for(int i = 1; i < 20; ++i) {
        BOOST_REQUIRE_EQUAL(hist.mean(i), 0.0f);
    }

    hist.tick(5);
    hist.tick(5);
    hist.tick(5);
    hist.tick(5);
    hist.tick(5);

    BOOST_REQUIRE_EQUAL(hist.min(5), 5.0f);
    BOOST_REQUIRE_EQUAL(hist.max(5), 5.0f);
    BOOST_REQUIRE_EQUAL(hist.deriv(5), 0);
    BOOST_REQUIRE_EQUAL(hist.mean(5), 5.0f);

    BOOST_REQUIRE_EQUAL(hist.min(10), 0.0f);
    BOOST_REQUIRE_EQUAL(hist.max(10), 5.0f);
    BOOST_REQUIRE_EQUAL(hist.deriv(10), 5.0f / 9.0f);
    BOOST_REQUIRE_EQUAL(hist.mean(10), 2.5f);

    hist.tick(6);
    hist.tick(7);
    hist.tick(8);
    hist.tick(9);
    hist.tick(10);

    BOOST_REQUIRE_EQUAL(hist.min(10), 5.0f);
    BOOST_REQUIRE_EQUAL(hist.max(10), 10.0f);
    BOOST_REQUIRE_EQUAL(hist.deriv(10), 5.0f / 9.0f);
    BOOST_REQUIRE_EQUAL(hist.mean(10), 6.5f);

    BOOST_REQUIRE_EQUAL(hist.min(20), 0.0f);
    BOOST_REQUIRE_EQUAL(hist.max(20), 10.0f);
    BOOST_REQUIRE_EQUAL(hist.deriv(20), 10.0f / 19.0f);
    BOOST_REQUIRE_EQUAL(hist.mean(20), 3.25f);

    hist.tick(11);
    hist.tick(12);
    hist.tick(13);
    hist.tick(14);
    hist.tick(15);
    hist.tick(16);
    hist.tick(17);
    hist.tick(18);
    hist.tick(19);
    hist.tick(20);

    BOOST_REQUIRE_EQUAL(hist.min(5), 16.0f);
    BOOST_REQUIRE_EQUAL(hist.max(5), 20.0f);
    BOOST_REQUIRE_EQUAL(hist.deriv(5), 1.0f);
    BOOST_REQUIRE_EQUAL(hist.mean(10), 15.5f);

    BOOST_REQUIRE_EQUAL(hist.min(20), 5.0f);
    BOOST_REQUIRE_EQUAL(hist.max(20), 20.0f);
    BOOST_REQUIRE_EQUAL(hist.deriv(20), 0.789473712f);
    BOOST_REQUIRE_EQUAL(hist.mean(20), 11.0f);
}

BOOST_AUTO_TEST_CASE( test_sp_util_lookback_large) {

    fixed_lookback<float> hist(100);

    for(int i = 0; i < 100000; ++i) {
        hist.tick(i);
    }

    BOOST_REQUIRE_EQUAL(hist.mean(99), 99950);
}
BOOST_AUTO_TEST_CASE( test_sp_util_lookback_running ) {

    rolling_lookback<float> hist(0.1);

    for(int i = 1; i <= 10; ++i) {
        hist.tick(i);
    }

    BOOST_REQUIRE_EQUAL(hist.mean(), 4.13810539f);

}


