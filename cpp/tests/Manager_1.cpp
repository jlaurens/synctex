/**
 * Manager_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */


#include <iostream>

#include "Manager.hpp"

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Manager");

TEST_CASE("POC") {
    CHECK_EQ(1, 1);
}

TEST_CASE("0") {
    auto m_p = Manager::make_p("0", "", true);
    int i = 0;
    CHECK_EQ(m_p->parse(i), Status::Done);
    CHECK_EQ(i, 0);
    auto s_p = m_p->sheet_p(1);
    CHECK(s_p); 
    auto ans = m_p->edit(1,0,0);
    CHECK(ans.empty());
    SYNCTEX_COVERAGE_REQUIRE(Manager::edit);
    // SYNCTEX_COVERAGE_REQUIRE(Manager::edit/s_p);
    // SYNCTEX_COVERAGE_REQUIRE(Manager::edit/b_p);
}

TEST_SUITE_END();
