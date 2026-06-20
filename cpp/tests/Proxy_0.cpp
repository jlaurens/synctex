/**
 * Proxy_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include "Proxy.hpp"

using namespace SyncTeXpp;

TEST_SUITE_BEGIN("Proxy");

TEST_CASE("POC") {
    CHECK_EQ(1, 1);
}

TEST_CASE("HBox") {
    auto b_p = HBox::make_p(
        1, 2, 3,
        10, 200,
        3000, 40000, 500000
    );
    auto r_p = Ref::make_p(6000000, 70000000, 800000000);
    auto prx = Proxy::make_p(r_p, b_p);
    CHECK_EQ(b_p->t(), 1);
    CHECK_EQ(r_p->t(), 6000000);
    CHECK_EQ(prx->t(), 1);
    CHECK_EQ(b_p->l(), 2);
    CHECK_EQ(r_p->l(), 0);
    CHECK_EQ(prx->l(), 2);
    CHECK_EQ(b_p->c(), 3);
    CHECK_EQ(r_p->c(), 0);
    CHECK_EQ(prx->c(), 3);
    CHECK_EQ(b_p->h(), 10);
    CHECK_EQ(r_p->h(), 70000000);
    CHECK_EQ(prx->h(), 70000000);
    CHECK_EQ(b_p->v(), 200);
    CHECK_EQ(r_p->v(), 800000000);
    CHECK_EQ(prx->v(), 800000000-40000);
}

TEST_CASE("VBox") {
    auto b_p = VBox::make_p(
        1, 2, 3,
        10, 200,
        3000, 40000, 500000
    );
    auto r_p = Ref::make_p(6000000, 70000000, 800000000);
    auto prx = Proxy::make_p(r_p, b_p);
    CHECK_EQ(b_p->t(), 1);
    CHECK_EQ(r_p->t(), 6000000);
    CHECK_EQ(prx->t(), 1);
    CHECK_EQ(b_p->l(), 2);
    CHECK_EQ(r_p->l(), 0);
    CHECK_EQ(prx->l(), 2);
    CHECK_EQ(b_p->c(), 3);
    CHECK_EQ(r_p->c(), 0);
    CHECK_EQ(prx->c(), 3);
    CHECK_EQ(b_p->h(), 10);
    CHECK_EQ(r_p->h(), 70000000);
    CHECK_EQ(prx->h(), 70000000);
    CHECK_EQ(b_p->v(), 200);
    CHECK_EQ(r_p->v(), 800000000);
    CHECK_EQ(prx->v(), 800000000-40000);
    CHECK_EQ(b_p->W(), 3000);
    CHECK_EQ(r_p->W(), 0);
    CHECK_EQ(prx->W(), 3000);
    CHECK_EQ(b_p->H(), 40000);
    CHECK_EQ(r_p->H(), 0);
    CHECK_EQ(prx->H(), 40000);
    CHECK_EQ(b_p->D(), 500000);
    CHECK_EQ(r_p->D(), 0);
    CHECK_EQ(prx->D(), 500000);
}

TEST_SUITE_END();
