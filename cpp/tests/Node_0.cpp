/**
 * Node_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It implements the class for representing nodes in a SyncTeX tree.
 * 
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include <sstream>

#include "SyncTeXNode.hpp"

namespace SyncTeXpp {

class TestNode;
using TestNode_p = std::shared_ptr<TestNode>;

class TestNode: public Node
{
    public:
    TestNode(): Node() {}
    std::ostream& log(std::ostream& os) const override {
        return os << "<TestNode>";
    };
};

std::ostream& operator<<(std::ostream& os, const TestNode_p& p) {
    if (p)
        return os << "Ptr to: " << *p;
    else
        return os << "nullptr";
}


class TestNode2: public Node
{
    public:
    TestNode2(): Node() {};
    int t() const override { return   1; }
    int l() const override { return   2; }
    int c() const override { return   3; }
    int h() const override { return  10; }
    int v() const override { return  20; }
    int W() const override { return 100; }
    int H() const override { return 200; }
    int D() const override { return 300; }
};

};

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Node");

TEST_CASE("Node tree")  {
    auto n = new TestNode();
    CHECK_EQ(nullptr, n->above_p());
    CHECK_EQ(nullptr, n->below_p());
    CHECK_EQ(nullptr, n->rbelow_p());
    CHECK_EQ(nullptr, n->left_p());
    CHECK_EQ(nullptr, n->right_p());
}

TEST_CASE("Node data")  {
    auto n = new TestNode();
    CHECK_EQ(0, n->t());
    CHECK_EQ(0, n->l());
    CHECK_EQ(0, n->c());

    CHECK_EQ(0, n->h());
    CHECK_EQ(0, n->v());
    CHECK_EQ(0, n->W());
    CHECK_EQ(0, n->H());
    CHECK_EQ(0, n->D());

    CHECK_EQ(0, n->w());
    CHECK_EQ(0, n->n());
    CHECK_EQ(0, n->e());
    CHECK_EQ(0, n->s());
}

TEST_CASE("Node data 2")  {
    auto n = new TestNode2();
    CHECK_EQ(  1, n->t());
    CHECK_EQ(  2, n->l());
    CHECK_EQ(  3, n->c());

    CHECK_EQ( 10, n->h());
    CHECK_EQ( 20, n->v());
    CHECK_EQ(100, n->W());
    CHECK_EQ(200, n->H());
    CHECK_EQ(300, n->D());
}

TEST_CASE("Node combo")  {
    auto n = new TestNode();
    CHECK_EQ(n->tlc(), tlc_s());
    CHECK_EQ(n->hv(), hv_s());
    CHECK_EQ(n->tlc_hv(), tlc_hv_s());
}

TEST_CASE("Node combo 2")  {
    auto n = new TestNode2();
    CHECK_EQ(n->tlc(), tlc_s(1,2,3));
    CHECK_EQ(n->hv(), hv_s(10,20));
    CHECK_EQ(n->tlc_hv(), tlc_hv_s(1,2,3,10,20));
}
TEST_CASE("isBox") {
    auto n = new TestNode2();
    CHECK(!n->isBox());
}

TEST_CASE("cout") {
    auto n = std::make_shared<TestNode>();
    std::ostringstream oss;
    oss << n;
    oss << "==";
    oss << *n;
    CHECK_EQ("Ptr to: <TestNode>==<TestNode>", oss.str());

}

TEST_SUITE_END();

