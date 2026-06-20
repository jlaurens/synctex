/**
 * Config_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It implements the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include "TestUtils.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

int foo() { return 421; }

TEST_CASE("Testing foo") {
    CHECK(421 == 421);
    CHECK(foo() == 421);
}

TEST_CASE("Coverage") {
    SyncTeXpp::Test::Coverage::enable();
    SYNCTEX_COVERAGE_PUSH(foo 1);
    SYNCTEX_COVERAGE(foo 2);
    SYNCTEX_COVERAGE_CHECK(foo 1);
    SYNCTEX_COVERAGE_CHECK(foo 2);
    SYNCTEX_COVERAGE_CHECK(foo 1, foo 2);
    SYNCTEX_COVERAGE_CHECK_NOT(foo 2, foo 1);
    SYNCTEX_COVERAGE_PUSH(foo 3, foo 4);
    SYNCTEX_COVERAGE_CHECK(foo 1, foo 3);
    SYNCTEX_COVERAGE_CHECK(foo 1, foo 3, foo 4);
    SYNCTEX_COVERAGE_CHECK(foo 1, "foo 3", foo 4);
    SYNCTEX_COVERAGE_CHECK_NOT(foo 1, "foo  3", foo 4);
    SyncTeXpp::Test::Coverage::disable();
    SYNCTEX_COVERAGE_PRINT("foo 1");
    SYNCTEX_COVERAGE_POP();
    SYNCTEX_COVERAGE_CHECK_NOT(foo 1, foo 4);
    SYNCTEX_COVERAGE("foo  5");
    SYNCTEX_COVERAGE_CHECK("foo  5");
    SYNCTEX_COVERAGE_CHECK_NOT(foo  5);
    SYNCTEX_COVERAGE_POP();
    SYNCTEX_COVERAGE_CHECK_NOT(foo 1);
    SYNCTEX_COVERAGE_CHECK_NOT(foo 2);
}
