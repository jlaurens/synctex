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
    m_p->parse(i);
    CHECK_EQ(i, 0);
}

TEST_SUITE_END();
