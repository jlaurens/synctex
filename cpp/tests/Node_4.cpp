/**
 * Node_4.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include <cassert>
#include "SyncTeX_tlc_hv_WHD.hpp"

namespace SyncTeXpp {

class TestBelow_lc_hv: public Below_lc_hv
{
    public:
    TestBelow_lc_hv(
        int t = 0,
        int l = 0,
        int c = 0,
        int h = 0,
        int v = 0
    ): Below_lc_hv(
        t, l, c,
        h, v) {}
};

class TestBelow_lc_hv_W: public Below_lc_hv_W
{
    public:
    TestBelow_lc_hv_W(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0
    ): Below_lc_hv_W(
        t, l, c,
        h, v,
        W
    ) {}
};

class TestBelow_lc_hv_WHD: public Below_lc_hv_WHD
{
    public:
    TestBelow_lc_hv_WHD(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ): Below_lc_hv_WHD(
        t, l, c,
        h, v,
        W, H, D
    ) {}
};

class TestAbove_lc_hv_WHD: public Above_lc_hv_WHD
{
    public:
    TestAbove_lc_hv_WHD(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ): Above_lc_hv_WHD(
        t, l, c,
        h, v,
        W, H, D
    ) {}
    void set_W(int v) { _W = v; }
    void set_H(int v) { _H = v; }
    void set_D(int v) { _D = v; }
};

};

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Below[B]_lc_hv[_WHD]");

TEST_CASE("TRIVIAL")  {
    CHECK(1==1);
}

TEST_CASE("Below_lc_hv 0")  {
    auto p = std::make_shared<TestBelow_lc_hv>();
    CHECK(p->t() == 0);
    CHECK(p->l() == 0);
    CHECK(p->c() == 0);
    CHECK(p->h() == 0);
    CHECK(p->v() == 0);
}

TEST_CASE("Below_lc_hv 1")  {
    auto p = std::make_shared<TestBelow_lc_hv>(
        1, 2, 3, 11, 12
    );
    CHECK(p->t() == 1);
    CHECK(p->l() == 2);
    CHECK(p->c() == 3);
    CHECK(p->h() == 11);
    CHECK(p->v() == 12);
}

#define SYNCTEX_TEST_CASE(WHAT, TT, LL, CC, HH, VV) \
TEST_CASE("Below_lc_hv 2")  {                       \
    auto p = std::make_shared<TestBelow_lc_hv>(     \
        1, 2, 3, 11, 12     \
    );                      \
    p->set_##WHAT(421);     \
    CHECK(p->t() == TT);    \
    CHECK(p->l() == LL);    \
    CHECK(p->c() == CC);    \
    CHECK(p->h() == HH);    \
    CHECK(p->v() == VV);    \
}

SYNCTEX_TEST_CASE(t, 421, 2, 3, 11, 12)
SYNCTEX_TEST_CASE(l, 1, 421, 3, 11, 12)
SYNCTEX_TEST_CASE(c, 1, 2, 421, 11, 12)
SYNCTEX_TEST_CASE(h, 1, 2, 3, 421, 12)
SYNCTEX_TEST_CASE(v, 1, 2, 3, 11, 421)

#undef SYNCTEX_TEST_CASE

TEST_CASE("Below_lc_hv_WHD 0")  {
    auto p = std::make_shared<TestBelow_lc_hv_WHD>();
    CHECK(p->t() == 0);
    CHECK(p->l() == 0);
    CHECK(p->c() == 0);
    CHECK(p->h() == 0);
    CHECK(p->v() == 0);
    CHECK(p->W() == 0);
    CHECK(p->H() == 0);
    CHECK(p->D() == 0);
}

TEST_CASE("Below_lc_hv_WHD 1")  {
    auto p = std::make_shared<TestBelow_lc_hv_WHD>(
        1, 2, 3, 11, 12, 101, 102, 103
    );
    CHECK(p->t() == 1);
    CHECK(p->l() == 2);
    CHECK(p->c() == 3);
    CHECK(p->h() == 11);
    CHECK(p->v() == 12);
    CHECK(p->W() == 101);
    CHECK(p->H() == 102);
    CHECK(p->D() == 103);
    p->set_t(421);
    CHECK(p->t() == 421);
    CHECK(p->l() == 2);
    CHECK(p->c() == 3);
    CHECK(p->h() == 11);
    CHECK(p->v() == 12);
    CHECK(p->W() == 101);
    CHECK(p->H() == 102);
    CHECK(p->D() == 103);
}

#define SYNCTEX_TEST_CASE(WHAT, TT, LL, CC, HH, VV, ww, hh, dd) \
TEST_CASE("Below_lc_hv_WHD 2")  {                               \
    auto p = std::make_shared<TestBelow_lc_hv_WHD>(             \
        1, 2, 3, 11, 12, 101, 102, 103                          \
    );                      \
    p->set_##WHAT(421);     \
    CHECK(p->t() == TT);    \
    CHECK(p->l() == LL);    \
    CHECK(p->c() == CC);    \
    CHECK(p->h() == HH);    \
    CHECK(p->v() == VV);    \
    CHECK(p->W() == ww);    \
    CHECK(p->H() == hh);    \
    CHECK(p->D() == dd);    \
}

SYNCTEX_TEST_CASE(t, 421, 2, 3, 11, 12, 101, 102, 103)
SYNCTEX_TEST_CASE(l, 1, 421, 3, 11, 12, 101, 102, 103)
SYNCTEX_TEST_CASE(c, 1, 2, 421, 11, 12, 101, 102, 103)
SYNCTEX_TEST_CASE(h, 1, 2, 3, 421, 12, 101, 102, 103)
SYNCTEX_TEST_CASE(v, 1, 2, 3, 11, 421, 101, 102, 103)
SYNCTEX_TEST_CASE(W, 1, 2, 3, 11, 12, 421, 102, 103)
SYNCTEX_TEST_CASE(H, 1, 2, 3, 11, 12, 101, 421, 103)
SYNCTEX_TEST_CASE(D, 1, 2, 3, 11, 12, 101, 102, 421)

#undef SYNCTEX_TEST_CASE

TEST_CASE("Above 0")  {
    auto p = std::make_shared<TestAbove_lc_hv_WHD>();
    CHECK(p->t() == 0);
    CHECK(p->l() == 0);
    CHECK(p->c() == 0);
    CHECK(p->h() == 0);
    CHECK(p->v() == 0);
}

TEST_CASE("Above_lc_hv_WHD 1")  {
    auto p = std::make_shared<TestAbove_lc_hv_WHD>(
        1, 2, 3, 11, 12, 101, 102, 103
    );
    CHECK(p->t() == 1);
    CHECK(p->l() == 2);
    CHECK(p->c() == 3);
    CHECK(p->h() == 11);
    CHECK(p->v() == 12);
}

#define SYNCTEX_TEST_CASE(WHAT, TT, LL, CC, HH, VV, ww, hh, dd) \
TEST_CASE("Above 2")  {                                         \
    auto p = std::make_shared<TestAbove_lc_hv_WHD>(             \
        1, 2, 3, 11, 12, 101, 102, 103                          \
    );                      \
    p->set_##WHAT(421);     \
    CHECK(p->t() == TT);    \
    CHECK(p->l() == LL);    \
    CHECK(p->c() == CC);    \
    CHECK(p->h() == HH);    \
    CHECK(p->v() == VV);    \
    CHECK(p->W() == ww);    \
    CHECK(p->H() == hh);    \
    CHECK(p->D() == dd);    \
}

SYNCTEX_TEST_CASE(t, 421, 2, 3, 11, 12, 101, 102, 103)
SYNCTEX_TEST_CASE(l, 1, 421, 3, 11, 12, 101, 102, 103)
SYNCTEX_TEST_CASE(c, 1, 2, 421, 11, 12, 101, 102, 103)
SYNCTEX_TEST_CASE(h, 1, 2, 3, 421, 12, 101, 102, 103)
SYNCTEX_TEST_CASE(v, 1, 2, 3, 11, 421, 101, 102, 103)
SYNCTEX_TEST_CASE(W, 1, 2, 3, 11, 12, 421, 102, 103)
SYNCTEX_TEST_CASE(H, 1, 2, 3, 11, 12, 101, 421, 103)
SYNCTEX_TEST_CASE(D, 1, 2, 3, 11, 12, 101, 102, 421)

#undef SYNCTEX_TEST_CASE

TEST_SUITE_END();

