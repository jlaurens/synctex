/**
 * Tree_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It implements the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include <memory>

#include "Tree.hpp"

namespace SyncTeXpp {

template <typename T>
class TestPOC: public T
{
    public:
    TestPOC(int t = 0): T(t) {}
    void set_t_doubled(int t) { T::set_t(2*t); }
};

};

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Tree");

TEST_CASE("Tag")  {
    auto below = TestBelow::make_p(123);
    CHECK_EQ(below->t(), 123);
    below->set_t(456);
    CHECK_EQ(below->t(), 456);
    auto above = TestAbove::make_p(321);
    CHECK_EQ(above->t(), 321);
    above->set_t(654);
    CHECK_EQ(above->t(), 654);
    TestPOC<Below> belowT(789);
    CHECK_EQ(belowT.t(), 789);
    belowT.set_t_doubled(234);
    CHECK_EQ(belowT.t(), 468);
}

TEST_CASE("Left/Right")  {
    auto below1 = TestBelow::make_p(1);
    auto below2 = TestBelow::make_p(2);
    auto below3 = TestBelow::make_p(3);

    SYNCTEX_CHECK_ALR(below1, nullptr, nullptr, nullptr);
    SYNCTEX_CHECK_ALR(below2, nullptr, nullptr, nullptr);
    SYNCTEX_CHECK_ALR(below3, nullptr, nullptr, nullptr);

    CHECK_EQ(below2, below1->set_right_p(below2));
    CHECK(! below1->picked());
    CHECK(! below2->picked());
    CHECK(  below3->picked());
    SYNCTEX_CHECK_ALR(below1, nullptr, nullptr, below2);
    SYNCTEX_CHECK_ALR(below2, nullptr, below1, nullptr);
    SYNCTEX_CHECK_ALR(below3, nullptr, nullptr, nullptr);

    CHECK_EQ(below2, below1->set_right_p(below3));
    SYNCTEX_CHECK_ALR(below1, nullptr, nullptr, below2);
    SYNCTEX_CHECK_ALR(below2, nullptr, below1, nullptr);
    SYNCTEX_CHECK_ALR(below3, nullptr, nullptr, nullptr);

    CHECK(! below2->picked());
    CHECK_EQ(nullptr, below3->set_right_p(below2));
    SYNCTEX_CHECK_ALR(below1, nullptr, nullptr, below2);
    SYNCTEX_CHECK_ALR(below2, nullptr, below1, nullptr);
    SYNCTEX_CHECK_ALR(below3, nullptr, nullptr, nullptr);

    CHECK_EQ(below1, below3->set_right_p(below1));
    SYNCTEX_CHECK_ALR(below1, nullptr, below3, below2);
    SYNCTEX_CHECK_ALR(below2, nullptr, below1, nullptr);
    SYNCTEX_CHECK_ALR(below3, nullptr, nullptr, below1);

    CHECK_EQ(below1, below3->set_right_p(below2));
    SYNCTEX_CHECK_ALR(below1, nullptr, below3, below2);
    SYNCTEX_CHECK_ALR(below2, nullptr, below1, nullptr);
    SYNCTEX_CHECK_ALR(below3, nullptr, nullptr, below1);
    CHECK_EQ(below1, below3->set_right_p(below3));
    SYNCTEX_CHECK_ALR(below1, nullptr, below3, below2);
    SYNCTEX_CHECK_ALR(below2, nullptr, below1, nullptr);
    SYNCTEX_CHECK_ALR(below3, nullptr, nullptr, below1);

    CHECK_EQ(nullptr, below2->set_right_p(below3));
    SYNCTEX_CHECK_ALR(below1, nullptr, below3, below2);
    SYNCTEX_CHECK_ALR(below2, nullptr, below1, nullptr);
    SYNCTEX_CHECK_ALR(below3, nullptr, nullptr, below1);
}


TEST_CASE("Above/Left/Right 123")  {
    auto above1 = TestAbove::make_p(10);

    auto below1 = TestBelow::make_p(1);
    auto below2 = TestBelow::make_p(2);
    auto below3 = TestBelow::make_p(3);

    CHECK_EQ(below1, above1->set_below_p(below1));
    SYNCTEX_CHECK_ALBRr(above1, nullptr, nullptr, below1, nullptr, below1);
    SYNCTEX_CHECK_ALR (below1, above1, nullptr, nullptr);

    CHECK_EQ(below2, below1->set_right_p(below2));
    SYNCTEX_CHECK_ALBRr(above1, nullptr, nullptr, below1, nullptr, below2);
    SYNCTEX_CHECK_ALR (below1, above1, nullptr, below2);
    SYNCTEX_CHECK_ALR (below2, above1, below1, nullptr);

    CHECK_EQ(below3, below2->set_right_p(below3));
    SYNCTEX_CHECK_ALBRr(above1, nullptr, nullptr, below1, nullptr, below3);
    SYNCTEX_CHECK_ALR (below1, above1, nullptr, below2);
    SYNCTEX_CHECK_ALR (below2, above1, below1, below3);
    SYNCTEX_CHECK_ALR (below3, above1, below2, nullptr);

}

TEST_CASE("Above/Left/Right 231")  {
    auto above1 = TestAbove::make_p(10);

    auto below1 = TestBelow::make_p(1);
    auto below2 = TestBelow::make_p(2);
    auto below3 = TestBelow::make_p(3);

    CHECK_EQ(below2, above1->set_below_p(below2));
    SYNCTEX_CHECK_ALBRr(above1, nullptr, nullptr, below2, nullptr, below2);
    SYNCTEX_CHECK_ALR (below2, above1, nullptr, nullptr);

    CHECK_EQ(below3, below2->set_right_p(below3));
    SYNCTEX_CHECK_ALBRr(above1, nullptr, nullptr, below2, nullptr, below3);
    SYNCTEX_CHECK_ALR (below2, above1, nullptr, below3);
    SYNCTEX_CHECK_ALR (below3, above1, below2, nullptr);

    CHECK_EQ(below1, above1->set_below_p(below1));
    SYNCTEX_CHECK_ALBRr(above1, nullptr, nullptr, below1, nullptr, below3);
    SYNCTEX_CHECK_ALR (below1, above1, nullptr, below2);
    SYNCTEX_CHECK_ALR (below2, above1, below1, below3);
    SYNCTEX_CHECK_ALR (below3, above1, below2, nullptr);
}

TEST_CASE("this")  {
    auto above1 = TestAbove::make_p(10);
    auto below1 = TestBelow::make_p(1);

    SYNCTEX_CHECK_ALBRr(above1, nullptr, nullptr, nullptr, nullptr, nullptr);
    SYNCTEX_CHECK_ALR (below1, nullptr, nullptr, nullptr);
  
    CHECK_EQ(nullptr, above1->set_below_p(above1));
    SYNCTEX_CHECK_ALBRr(above1, nullptr, nullptr, nullptr, nullptr, nullptr);
    SYNCTEX_CHECK_ALR (below1, nullptr, nullptr, nullptr);
 
    CHECK_EQ(nullptr, above1->set_right_p(above1));
    SYNCTEX_CHECK_ALBRr(above1, nullptr, nullptr, nullptr, nullptr, nullptr);
    SYNCTEX_CHECK_ALR (below1, nullptr, nullptr, nullptr);

    CHECK_EQ(nullptr, below1->set_right_p(below1));
    SYNCTEX_CHECK_ALBRr(above1, nullptr, nullptr, nullptr, nullptr, nullptr);
    SYNCTEX_CHECK_ALR (below1, nullptr, nullptr, nullptr);
}

TEST_SUITE_END();

