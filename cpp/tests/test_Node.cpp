/**
 * SyncTeXNode.cpp
 * 
 * This file is part of the SyncTeX library.
 * It implements the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#define SYNCTEX_WITH_TEST
#include "SyncTeXNode.cpp"

int add(int a, int b) { return a + b; }

TEST_CASE("Testing add function") {
    CHECK(add(2, 3) == 5);
    CHECK(add(-1, 1) == 0);
}

