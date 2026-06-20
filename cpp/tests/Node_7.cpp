/**
 * Node_7.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */


#include "SyncTeXSheet.hpp"
#include "SyncTeXBox.hpp"
#include "SyncTeXNoBox.hpp"

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Sheet");

TEST_CASE("TRIVIAL") {
    CHECK_EQ(421, 421);
}

TEST_CASE("Constructor") {
    auto sheet = Sheet::make_p(123);
    CHECK_EQ(sheet->t(), 123);
    CHECK_EQ(sheet->level(), -1);
}

TEST_CASE("Tree") {
    auto sheet1 = Sheet::make_p(1);
    auto m = Math::make_p();
    sheet1->set_below_p(m);
    CHECK_EQ(nullptr, sheet1->below_box());
    auto sheet2 = Sheet::make_p(2);
    auto h = HBox::make_p();
    sheet2->set_below_p(h);
    CHECK_EQ(h, sheet2->below_box());
    CHECK(sheet2->below_box()->isBox());
    auto sheet3 = Sheet::make_p(3);
    auto v = VBox::make_p();
    sheet3->set_below_p(v);
    CHECK_EQ(v, sheet3->below_box());
    CHECK(sheet3->below_box()->isBox());
}

TEST_CASE("Above_p <= Sheet_p") {
    Sheet_p s_p = Sheet::make_p();
    Above_p above_p = s_p;
    CHECK_EQ(above_p, s_p);
}

TEST_SUITE_END();

