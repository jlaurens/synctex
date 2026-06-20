/**
 * Node_6.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include "TestUtils.hpp"

#include <ostream>
#include "SyncTeXBox.hpp"
#include "SyncTeXNoBox.hpp"
#include "Tree.hpp"

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Box");

TEST_CASE("TRIVIAL")  {
    CHECK(1==1);
}

#undef  SYNCTEX_TEST_CASE
#define SYNCTEX_TEST_CASE(WHAT, PREFIX, SUFFIX) \
TEST_CASE(SYNCTEX_X_STRINGIFY(WHAT)) {           \
    auto p = WHAT::make_p(    \
        1, 2, 3,               \
        10, 200,                \
        3000, 40000, 500000      \
    );                            \
    std::ostringstream oss1, oss2; \
    oss1 << p;             \
    oss2 << PREFIX          \
        << p->t() << ","     \
        << p->l() << ","      \
        << p->c() << ":"       \
        << p->h() << ","        \
        << p->v() << ":"         \
        << p->W() << ","          \
        << p->H() << ","           \
        << p->D()                   \
        << SUFFIX;                   \
    CHECK_EQ(oss1.str(), oss2.str()); \
    CHECK_EQ(p->t(),      1); \
    CHECK_EQ(p->l(),      2);  \
    CHECK_EQ(p->c(),      3);   \
    CHECK_EQ(p->h(),     10);    \
    CHECK_EQ(p->v(),    200);     \
    CHECK_EQ(p->W(),   3000);      \
    CHECK_EQ(p->H(),  40000);       \
    CHECK_EQ(p->D(), 500000);        \
    CHECK_EQ(p->w(),     10);         \
    CHECK_EQ(p->n(), -39800);          \
    CHECK_EQ(p->e(),   3010);           \
    CHECK_EQ(p->s(), 500200);            \
}

SYNCTEX_TEST_CASE(HBox, "(", "\n)");
SYNCTEX_TEST_CASE(VBox, "[", "\n]");

#undef SYNCTEX_TEST_CASE

TEST_CASE("Box level") {
    auto b0 = HBox::make_p();
    auto b1 = VBox::make_p();
    auto b2 = HBox::make_p();
    auto b3 = VBox::make_p();
    b0->set_below_p(b1);
    b1->set_below_p(b2);
    b2->set_below_p(b3);
    CHECK_EQ(0, b0->level());
    CHECK_EQ(1, b1->level());
    CHECK_EQ(2, b2->level());
    CHECK_EQ(3, b3->level());
    auto m3 = Math::make_p();
    auto m4 = Math::make_p();
    b3->set_right_p(m3);
    CHECK_EQ(3, m3->level());
    b3->set_below_p(m4);
    CHECK_EQ(4, m4->level());
}

TEST_CASE("dynamic_cast VBox") {
    std::shared_ptr<Box> b_v = VBox::make_p();
    auto v = std::dynamic_pointer_cast<VBox>(b_v);
    CHECK(v);
    std::shared_ptr<Box> b_h = HBox::make_p();
    auto h = std::dynamic_pointer_cast<HBox>(b_h);
    CHECK(h);
}

TEST_CASE("make_bdries") {
    auto below1 = Ref::make_p(1, 100, 1000);
    auto below2 = Math::make_p(2, 20, 0, 200, 2000);
    auto below3 = Math::make_p(3, 30, 0, 300, 3000);
    auto below4 = Ref::make_p(4, 400, 4000);
    auto below5 = Ref::make_p(5, 500, 5000);
    auto hbox_p = HBox::make_p(6, 60, 0, 600, 6000, 60000, 600000, 6000000);
    hbox_p->set_below_p(below1);
    below1->set_right_p(below2);
    below2->set_right_p(below3);
    below3->set_right_p(below4);
    below4->set_right_p(below5);
    CHECK(below1->isRef());
    CHECK(!below2->isRef());
    CHECK(!below3->isRef());
    CHECK(below4->isRef());
    CHECK(below5->isRef());
    SYNCTEX_CHECK_ALR(below1, hbox_p, nullptr, below2);
    SYNCTEX_CHECK_ALR(below2, hbox_p, below1, below3);
    SYNCTEX_CHECK_ALR(below3, hbox_p, below2, below4);
    SYNCTEX_CHECK_ALR(below4, hbox_p, below3, below5);
    SYNCTEX_CHECK_ALR(below5, hbox_p, below4, nullptr);
    auto [first_p, last_p] = hbox_p->make_bdries();
    CHECK_EQ(first_p, hbox_p->below_p());
    CHECK_EQ(last_p, hbox_p->rbelow_p());
    CHECK_EQ(first_p->tlc(), below2->tlc());
    CHECK_EQ(last_p->tlc(), below3->tlc());
    SYNCTEX_CHECK_ALR(first_p, hbox_p, nullptr, below1);
    SYNCTEX_CHECK_ALR(below1, hbox_p, first_p, below2);
    SYNCTEX_CHECK_ALR(below2, hbox_p, below1, below3);
    SYNCTEX_CHECK_ALR(below3, hbox_p, below2, below4);
    SYNCTEX_CHECK_ALR(below4, hbox_p, below3, below5);
    SYNCTEX_CHECK_ALR(below5, hbox_p, below4, last_p);
    SYNCTEX_CHECK_ALR(last_p, hbox_p, below5, nullptr);
}

TEST_CASE("cast") {
    Below_p b_p = VBox::make_p(1, 10, 100, 1000, 10000, 100000, 1000000, 10000000);
    VBox_p vbox_p = b_p->asVBox_p();
    CHECK(vbox_p);
    CHECK_EQ(       1, vbox_p->t());
    CHECK_EQ(      10, vbox_p->l());
    CHECK_EQ(     100, vbox_p->c());
    CHECK_EQ(    1000, vbox_p->h());
    CHECK_EQ(   10000, vbox_p->v());
    CHECK_EQ(  100000, vbox_p->W());
    CHECK_EQ( 1000000, vbox_p->H());
    CHECK_EQ(10000000, vbox_p->D());
}

TEST_SUITE_END();

TEST_SUITE_BEGIN("Fit");

TEST_CASE("HBox contains") {
    auto p = HBox::make_p(0, 0, 0, 4, 50000, 60, 700, 8000);
    CHECK_EQ(p->wnes(), wnes_s(4, 50000 - 700, 4 + 60, 50000 + 8000));
    CHECK_EQ(p->wnes(), wnes_s(4, 49300, 64, 58000));
    CHECK(!p->contains({ 3, 49299}));
    CHECK(!p->contains({ 3, 49300}));
    CHECK(!p->contains({ 3, 58000}));
    CHECK(!p->contains({ 3, 58001}));
    CHECK(!p->contains({ 4, 49299}));
    CHECK( p->contains({ 4, 49300}));
    CHECK( p->contains({ 4, 58000}));
    CHECK(!p->contains({ 4, 58001}));
    CHECK(!p->contains({64, 49299}));
    CHECK( p->contains({64, 49300}));
    CHECK( p->contains({64, 58000}));
    CHECK(!p->contains({64, 58001}));
    CHECK(!p->contains({65, 49299}));
    CHECK(!p->contains({65, 49300}));
    CHECK(!p->contains({65, 58000}));
    CHECK(!p->contains({65, 58001}));
}

#undef SYNCTEX_CHECK
#define SYNCTEX_CHECK(H, V, W, N, E, S)\
    pp->set_h(H);\
    pp->set_v(V);\
    p->fit_to_hv_V(pp);\
    CHECK_EQ(p->wnes_V(), wnes_s(W, N, E, S))
TEST_CASE("HBox fit") {
    auto p = HBox::make_p(0, 0, 0, 4, 50000, 60, 700, 8000);
    auto pp = Math::make_p();
    SYNCTEX_CHECK( 4, 49300, 4, 49300, 64, 58000);
    SYNCTEX_CHECK( 3, 49300, 3, 49300, 64, 58000);
    SYNCTEX_CHECK( 3, 49299, 3, 49299, 64, 58000);
    SYNCTEX_CHECK(65, 58000, 3, 49299, 65, 58000);
    SYNCTEX_CHECK(65, 58001, 3, 49299, 65, 58001);
}
#undef SYNCTEX_CHECK
#define SYNCTEX_CHECK(H, V, W, N, E, S)\
    p1 = HBox::make_p(0, 0, 0, 4, 50000, 60, 700, 8000);\
    p2 = HBox::make_p(0, 0, 0, 4, 50000, 60, 700, 8000);\
    pp = Math::make_p(0, 0, 0, H, V);\
    p1->fit_to_hv_V(pp);\
    CHECK_EQ(p1->wnes_V(), wnes_s(W, N, E, S));\
    p2->fit_to_wnes_V(p1);\
    CHECK_EQ(p2->wnes_V(), wnes_s(W, N, E, S))
TEST_CASE("HBox fit") {
    auto p1 = HBox::make_p(0, 0, 0, 4, 50000, 60, 700, 8000);
    auto p2 = HBox::make_p(0, 0, 0, 4, 50000, 60, 700, 8000);
    auto pp = Math::make_p(0, 0, 0, -4, 49300);
    p1->fit_to_hv_V(pp);
    CHECK_EQ(p1->wnes_V(), wnes_s(-4, 49300, 64, 58000));
    p2->fit_to_wnes_V(p1);
    CHECK_EQ(p2->wnes_V(), wnes_s(-4, 49300, 64, 58000));
    

    SYNCTEX_CHECK( 4, 49300, 4, 49300, 64, 58000);
    SYNCTEX_CHECK( 3, 49300, 3, 49300, 64, 58000);
    SYNCTEX_CHECK( 3, 49299, 3, 49299, 64, 58000);
    SYNCTEX_CHECK(65, 58000, 4, 49300, 65, 58000);
    SYNCTEX_CHECK(65, 58001, 4, 49300, 65, 58001);
}
#undef SYNCTEX_CHECK

TEST_CASE("VBox wnes") {
    auto p = VBox::make_p(0, 0, 0, 4, 50000, 60, 700, 8000);
    CHECK_EQ(p->wnes(), wnes_s(4, 50000 - 700, 4 + 60, 50000 + 8000));
    CHECK_EQ(p->wnes(), wnes_s(4, 49300, 64, 58000));
    CHECK(!p->contains({ 3, 49299}));
    CHECK(!p->contains({ 3, 49300}));
    CHECK(!p->contains({ 3, 58000}));
    CHECK(!p->contains({ 3, 58001}));
    CHECK(!p->contains({ 4, 49299}));
    CHECK(!p->contains({ 4, 49300}));
    CHECK(!p->contains({ 4, 58000}));
    CHECK(!p->contains({ 4, 58001}));
    CHECK(!p->contains({64, 49299}));
    CHECK(!p->contains({64, 49300}));
    CHECK(!p->contains({64, 58000}));
    CHECK(!p->contains({64, 58001}));
    CHECK(!p->contains({65, 49299}));
    CHECK(!p->contains({65, 49300}));
    CHECK(!p->contains({65, 58000}));
    CHECK(!p->contains({65, 58001}));
}

TEST_SUITE_END();
