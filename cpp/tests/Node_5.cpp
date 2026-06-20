/**
 * Node_5.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include "TestUtils.hpp"

#include <random>
#include <algorithm>

std::mt19937 rng(std::random_device{}());  // seed with hardware entropy
std::uniform_int_distribution<int> distribution(-100, 100);  // inclusive range [1, 100]

#include "SyncTeXNoBox.hpp"

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("HV");

TEST_CASE("wnse 0")  {
    wnes_s wnes(
        10,
        10,
        -10,
        -10
    );
    CHECK_LE(wnes.w, wnes.e);
    CHECK_LE(wnes.n, wnes.s);
}

TEST_CASE("wnse")  {
    int i = 50;
    while (i--) {
        wnes_s wnes(
            distribution(rng),
            distribution(rng),
            distribution(rng),
            distribution(rng)
        );
        CHECK_LE(wnes.w, wnes.e);
        CHECK_LE(wnes.n, wnes.s);
    }
}

#define SYNCTEX_CHECK(HH,VV,dd) \
    CHECK_EQ(dd, Geo::distance_to(box, {HH, VV}))

TEST_CASE("Geo") {
/*
 *      W   <   E
 *    1 │   2   │ 3
 * N ───╆━━━━━━━╅─── 0
 * ^  4 ┃   5   ┃ 6
 * S ───╄━━━━━━━╃─── 10
 *    7 │   8   │ 9
 *      0      +10
 *
 */
    wnes_s box(0, 0, 10, 10);
    CHECK_EQ(box.w,  0);
    CHECK_EQ(box.n,  0);
    CHECK_EQ(box.e, 10);
    CHECK_EQ(box.s, 10);
    
    int med_h = (box.w+box.e)/2;
    int med_v = (box.n+box.s)/2;
    int delta_v = 10;
    SYNCTEX_CHECK(box.w-10, box.n-delta_v, 10+delta_v);
    SYNCTEX_CHECK(box.w-00, box.n-delta_v, 00+delta_v);
    SYNCTEX_CHECK(   med_h, box.n-delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+00, box.n-delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+20, box.n-delta_v, 20+delta_v);

    delta_v = 0;
    SYNCTEX_CHECK(box.w-10, box.n-delta_v, 10+delta_v);
    SYNCTEX_CHECK(box.w-00, box.n-delta_v, 00+delta_v);
    SYNCTEX_CHECK(   med_h, box.n-delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+00, box.n-delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+20, box.n-delta_v, 20+delta_v);

    SYNCTEX_CHECK(box.w-10, med_v, 10+delta_v);
    SYNCTEX_CHECK(box.w-00, med_v, 00+delta_v);
    SYNCTEX_CHECK(   med_h, med_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+00, med_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+20, med_v, 20+delta_v);

    SYNCTEX_CHECK(box.w-10, box.s+delta_v, 10+delta_v);
    SYNCTEX_CHECK(box.w-00, box.s+delta_v, 00+delta_v);
    SYNCTEX_CHECK(   med_h, box.s+delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+00, box.s+delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+20, box.s+delta_v, 20+delta_v);

    delta_v = 17;
    SYNCTEX_CHECK(box.w-10, box.s+delta_v, 10+delta_v);
    SYNCTEX_CHECK(box.w-00, box.s+delta_v, 00+delta_v);
    SYNCTEX_CHECK(   med_h, box.s+delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+00, box.s+delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+20, box.s+delta_v, 20+delta_v);

}

TEST_CASE("Geo") {
/*
 *      W   <   E
 *    1 │   2   │ 3
 * N ───╆━━━━━━━╅─── -5
 * ^  4 ┃   5   ┃ 6
 * S ───╄━━━━━━━╃─── +15
 *    7 │   8   │ 9
 *     -10     +20
 *
 */
    wnes_s box(-10, -5, 20, 15);
    CHECK_EQ(box.w, -10);
    CHECK_EQ(box.n, -5);
    CHECK_EQ(box.e,  20);
    CHECK_EQ(box.s,  15);
    
    int med_h = (box.w+box.e)/2;
    int med_v = (box.n+box.s)/2;
    int delta_v = 5;
    SYNCTEX_CHECK(box.w-10, box.n-delta_v, 10+delta_v);
    SYNCTEX_CHECK(box.w-00, box.n-delta_v, 00+delta_v);
    SYNCTEX_CHECK(   med_h, box.n-delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+00, box.n-delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+20, box.n-delta_v, 20+delta_v);

    SYNCTEX_CHECK(-15, -10, 10);
    SYNCTEX_CHECK(-10, -10,  5);
    SYNCTEX_CHECK(  5, -10,  5);
    SYNCTEX_CHECK( 20, -10,  5);
    SYNCTEX_CHECK( 30, -10, 15);

    delta_v = 0;
    SYNCTEX_CHECK(box.w-10, box.n-delta_v, 10+delta_v);
    SYNCTEX_CHECK(box.w-00, box.n-delta_v, 00+delta_v);
    SYNCTEX_CHECK(   med_h, box.n-delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+00, box.n-delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+20, box.n-delta_v, 20+delta_v);

    SYNCTEX_CHECK(box.w-10, med_v, 10+delta_v);
    SYNCTEX_CHECK(box.w-00, med_v, 00+delta_v);
    SYNCTEX_CHECK(   med_h, med_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+00, med_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+20, med_v, 20+delta_v);

    SYNCTEX_CHECK(box.w-10, box.s+delta_v, 10+delta_v);
    SYNCTEX_CHECK(box.w-00, box.s+delta_v, 00+delta_v);
    SYNCTEX_CHECK(   med_h, box.s+delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+00, box.s+delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+20, box.s+delta_v, 20+delta_v);

    delta_v = 17;
    SYNCTEX_CHECK(box.w-10, box.s+delta_v, 10+delta_v);
    SYNCTEX_CHECK(box.w-00, box.s+delta_v, 00+delta_v);
    SYNCTEX_CHECK(   med_h, box.s+delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+00, box.s+delta_v, 00+delta_v);
    SYNCTEX_CHECK(box.e+20, box.s+delta_v, 20+delta_v);


    SYNCTEX_CHECK(-15,  -5,  5);
    SYNCTEX_CHECK(-10,  -5,  0);
    SYNCTEX_CHECK(  5,  -5,  0);
    SYNCTEX_CHECK( 20,  -5,  0);
    SYNCTEX_CHECK( 30,  -5, 10);
    SYNCTEX_CHECK(-15,   5,  5);
    SYNCTEX_CHECK(-10,   5,  0);
    SYNCTEX_CHECK(  5,   5,  0);
    SYNCTEX_CHECK( 20,   5,  0);
    SYNCTEX_CHECK( 30,   5, 10);
    SYNCTEX_CHECK(-15,  15,  5);
    SYNCTEX_CHECK(-10,  15,  0);
    SYNCTEX_CHECK(  5,  15,  0);
    SYNCTEX_CHECK( 20,  15,  0);
    SYNCTEX_CHECK( 30,  15, 10);
    SYNCTEX_CHECK(-15,  25, 15);
    SYNCTEX_CHECK(-10,  25, 10);
    SYNCTEX_CHECK(  5,  25, 10);
    SYNCTEX_CHECK( 20,  25, 10);
    SYNCTEX_CHECK( 30,  25, 20);
}
#undef SYNCTEX_CHECK

TEST_CASE("Ref")  {
    // auto t = std::make_shared<TestBelow_lc_hv_W>(
    //     1, 2, 3, 10, 200, 3000
    // );
    // std::cout << "<<<" << t << ">>>" << std::endl;
    auto p = Ref::make_p(1, 10, 100);
    // std::cout << "<<<" << p << ">>>" << std::endl;
    CHECK_EQ(p->t(),    1);
    CHECK_EQ(p->l(),    0);
    CHECK_EQ(p->c(),    0);
    CHECK_EQ(p->h(),   10);
    CHECK_EQ(p->v(),  100);
    CHECK_EQ(p->W(),0);
    CHECK_EQ(p->w(),10);
    CHECK_EQ(p->n(),100);
    CHECK_EQ(p->e(),10);
    CHECK_EQ(p->s(),100);

}

#define SYNCTEX_CHECK(HH,VV,dd)        \
CHECK_EQ(dd, p->distance_to({HH,VV}))

#define SYNCTEX_TEST_CASE(WHAT)                         \
TEST_CASE(SYNCTEX_X_STRINGIFY(WHAT)) {                   \
    auto p = WHAT::make_p();                              \
    CHECK(p->type() == Node::Type::WHAT);                  \
    CHECK(p->type_name() == SYNCTEX_X_STDSTRINGIFY(WHAT));  \
    SYNCTEX_CHECK(  0,  0,  0); \
    SYNCTEX_CHECK( 10,  0,  10); \
    SYNCTEX_CHECK( 10,  20, 30);  \
    SYNCTEX_CHECK(  0,  20, 20);   \
    SYNCTEX_CHECK(-10,   0, 10);    \
    SYNCTEX_CHECK(-10,  20, 30);     \
    SYNCTEX_CHECK( 10, -20, 30);      \
    SYNCTEX_CHECK(  0, -20, 20);       \
    SYNCTEX_CHECK(-10, -20, 30);        \
}

SYNCTEX_TEST_CASE(Math)
SYNCTEX_TEST_CASE(Glue)
SYNCTEX_TEST_CASE(Bdry)
SYNCTEX_TEST_CASE(BoxBdry)

#undef SYNCTEX_TEST_CASE
#undef SYNCTEX_CHECK

#define SYNCTEX_CHECK(HH,VV,dd)    \
CHECK_EQ(dd, p->h_distance_to({HH,VV}))

#define SYNCTEX_TEST_CASE(WHAT)        \
TEST_CASE(SYNCTEX_X_STRINGIFY(WHAT)) {  \
    auto p = WHAT::make_p();    \
    SYNCTEX_CHECK(  0,   0,  0); \
    SYNCTEX_CHECK( 10,   0, 10);  \
    SYNCTEX_CHECK( 10,  20, 10);   \
    SYNCTEX_CHECK(  0,  20,  0);    \
    SYNCTEX_CHECK(-10,   0, 10);     \
    SYNCTEX_CHECK(-10,  20, 10);      \
    SYNCTEX_CHECK( 10, -20, 10);       \
    SYNCTEX_CHECK(  0, -20,  0);        \
    SYNCTEX_CHECK(-10, -20, 10);         \
}

SYNCTEX_TEST_CASE(Math)
SYNCTEX_TEST_CASE(Glue)
SYNCTEX_TEST_CASE(Bdry)
SYNCTEX_TEST_CASE(BoxBdry)

#undef SYNCTEX_TEST_CASE
#undef SYNCTEX_CHECK

#define SYNCTEX_CHECK(HH,VV,dd)    \
CHECK_EQ(dd, p->v_distance_to({HH,VV}))

#define SYNCTEX_TEST_CASE(WHAT)        \
TEST_CASE(SYNCTEX_X_STRINGIFY(WHAT)) {  \
    auto p = WHAT::make_p();   \
    Bi_s bi;                    \
    SYNCTEX_CHECK(  0,   0,  0); \
    SYNCTEX_CHECK( 10,   0,  0);  \
    SYNCTEX_CHECK( 10,  20, 20);   \
    SYNCTEX_CHECK(  0,  20, 20);    \
    SYNCTEX_CHECK(-10,   0,  0);     \
    SYNCTEX_CHECK(-10,  20, 20);      \
    SYNCTEX_CHECK( 10, -20, 20);       \
    SYNCTEX_CHECK(  0, -20, 20);        \
    SYNCTEX_CHECK(-10, -20, 20);         \
}

SYNCTEX_TEST_CASE(Math)
SYNCTEX_TEST_CASE(Glue)
SYNCTEX_TEST_CASE(Bdry)
SYNCTEX_TEST_CASE(BoxBdry)

#undef SYNCTEX_TEST_CASE
#undef SYNCTEX_CHECK

class TestBelow_lc_hv_W: public Below_lc_hv_W {
    public:
    char prefix() const override { return '?' ; }
    TestBelow_lc_hv_W(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0       
    ): Below_lc_hv_W(t, l, c, h, v, W) {}
};

using TestBelow_lc_hv_W_p = std::shared_ptr<TestBelow_lc_hv_W>;

std::ostream& operator<<(std::ostream& os, const TestBelow_lc_hv_W_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

TEST_CASE("TestBelow_lc_hv_W")  {
    auto p = std::make_shared<TestBelow_lc_hv_W>(
        1, 2, 3, 10, 200, 3000
    );
    CHECK_EQ(p->t(),    1);
    CHECK_EQ(p->l(),    2);
    CHECK_EQ(p->c(),    3);
    CHECK_EQ(p->h(),   10);
    CHECK_EQ(p->v(),  200);
    CHECK_EQ(p->W(), 3000);
    CHECK_EQ(p->w(),   10);
    CHECK_EQ(p->n(),  200);
    CHECK_EQ(p->e(), 3010);
    CHECK_EQ(p->s(),  200);

}

TEST_CASE("Kern")  {
    // auto t = std::make_shared<TestBelow_lc_hv_W>(
    //     1, 2, 3, 10, 200, 3000
    // );
    // std::cout << "<<<" << t << ">>>" << std::endl;
    auto p = Kern::make_p(1, 2, 3, 10, 200, 3000);
    // std::cout << "<<<" << p << ">>>" << std::endl;
    CHECK_EQ(p->t(),    1);
    CHECK_EQ(p->l(),    2);
    CHECK_EQ(p->c(),    3);
    CHECK_EQ(p->h(),   10);
    CHECK_EQ(p->v(),  200);
    CHECK_EQ(p->W(),-3000);
    CHECK_EQ(p->w(),-2990);
    CHECK_EQ(p->n(),  200);
    CHECK_EQ(p->e(),   10);
    CHECK_EQ(p->s(),  200);

}

#undef SYNCTEX_TEST_CASE

#define SYNCTEX_TEST_CASE(WHAT, PREFIX) \
TEST_CASE(SYNCTEX_X_STRINGIFY(WHAT)) {  \
    auto p = WHAT::make_p(              \
        1, 2, 3,                        \
        10, 200,                        \
        3000, 40000, 500000             \
    );                                  \
    std::ostringstream oss1, oss2;      \
    oss1 << p;                          \
    oss2 << PREFIX                      \
        << p->t() << ","                \
        << p->l() << ","                \
        << p->c() << ":"                \
        << p->h() << ","                \
        << p->v() << ":"                \
        << p->W() << ","                \
        << p->H() << ","                \
        << p->D();                      \
    CHECK_EQ(oss1.str(), oss2.str());   \
    CHECK_EQ(p->t(),      1);           \
    CHECK_EQ(p->l(),      2);           \
    CHECK_EQ(p->c(),      3);           \
    CHECK_EQ(p->h(),     10);           \
    CHECK_EQ(p->v(),    200);           \
    CHECK_EQ(p->W(),   3000);           \
    CHECK_EQ(p->H(),  40000);           \
    CHECK_EQ(p->D(), 500000);           \
    CHECK_EQ(p->w(),     10);           \
    CHECK_EQ(p->n(), -39800);           \
    CHECK_EQ(p->e(),   3010);           \
    CHECK_EQ(p->s(), 500200);           \
}

SYNCTEX_TEST_CASE(Rule, "r");
SYNCTEX_TEST_CASE(VoidHBox, "h");
SYNCTEX_TEST_CASE(VoidVBox, "v");

#undef SYNCTEX_TEST_CASE

#define SYNCTEX_TEST_CASE(WHAT)             \
TEST_CASE(SYNCTEX_TO_STRING(WHAT) " Geo") { \
    auto p = WHAT::make_p(                  \
        1, 2, 3,                            \
        10, 200,                            \
        3000, 40000, 500000                 \
    );                                      \
    std::ostringstream oss1, oss2;          \
    oss1 << p;                              \
    oss2 << PREFIX                          \
        << p->t() << ","                    \
        << p->l() << ","                    \
        << p->c() << ":"                    \
        << p->h() << ","                    \
        << p->v() << ":"                    \
        << p->W() << ","                    \
        << p->H() << ","                    \
        << p->D();                          \
    CHECK_EQ(oss1.str(), oss2.str());       \
    CHECK_EQ(p->t(),      1);               \
    CHECK_EQ(p->l(),      2);               \
    CHECK_EQ(p->c(),      3);               \
    CHECK_EQ(p->h(),     10);               \
    CHECK_EQ(p->v(),    200);               \
    CHECK_EQ(p->W(),   3000);               \
    CHECK_EQ(p->H(),  40000);               \
    CHECK_EQ(p->D(), 500000);               \
    CHECK_EQ(p->w(),     10);               \
    CHECK_EQ(p->n(), -39800);               \
    CHECK_EQ(p->e(),   3010);               \
    CHECK_EQ(p->s(), 500200);               \
}

TEST_CASE("BOX"  " Geo") {
    int i = 50;
    while (i--) {
        auto p = Rule::make_p(
            0, 0, 0,
            distribution(rng),
            distribution(rng),
            distribution(rng),
            distribution(rng),
            distribution(rng)
        );
        CHECK_LE(p->w(), p->e());
        CHECK_LE(p->n(), p->s());
    }
}

#undef SYNCTEX_TEST_CASE

TEST_SUITE_END();
