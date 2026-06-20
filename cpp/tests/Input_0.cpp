/**
 * Node_7.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include <set>
#include <utility>

#include "SyncTeXInput.hpp"
#include "Input.hpp"
#include "Tree.hpp"

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Input");

TEST_CASE("TRIVIAL") {
    CHECK_EQ(421, 421);
}

TEST_CASE("Constructor") {
    auto input_p = Input::make_p(123);
    CHECK_EQ(input_p->t(), 123);
}

/*
 * Input_dir
 * ├─ 0.txt
 * ├─ 1.txt
 * ├─ absolute_directory
 * │  ├─ 0.txt
 * │  ├─ 1.txt
 * │  └─ 2.txt
 * └─ output_directory
 *    ├─ 1.txt
 *    ├─ 2.txt
 *    └─ absolute_directory
 *       └─ 1.txt
 */
TEST_CASE("Path" * doctest::skip() ) {
    auto cwd = fs::canonical(".");
    {
        auto i = Input::make_p(123, "0.txt");
        CHECK_EQ(i->realname(), cwd / "0.txt");
    }
    {
        auto i = Input::make_p(123, "1.txt", "output_directory");
        CHECK_EQ(i->realname(), cwd / "output_directory" / "1.txt");
    }
    {
        auto i = Input::make_p(123, "absolute_directory/2.txt", cwd / "output_directory");
        CHECK_EQ(i->realname(), cwd / "output_directory" / "2.txt");
    }
    {
        auto i = Input::make_p(123, "absolute_directory/2.txt", "output_directory");
        CHECK_EQ(i->realname(), cwd / "output_directory" / "2.txt");
    }
    {
        auto i = Input::make_p(123, "absolute_directory/3.txt", cwd / "output_directory");
        CHECK_EQ(i->realname(), cwd / "output_directory" / "absolute_directory/3.txt");
    }
    {
        auto i = Input::make_p(123, "absolute_directory/3.txt", "output_directory");
        CHECK_EQ(i->realname(), cwd / "output_directory" / "absolute_directory/3.txt");
    }
    {
        auto i = Input::make_p(123, cwd / "0.txt");
        CHECK_EQ(i->realname(), cwd / "0.txt");
    }
    {
        auto i = Input::make_p(123, cwd / "1.txt");
        CHECK(!i->is_valid());
    }
    {
        auto i = Input::make_p(123, cwd / "1.txt", cwd / "output_directory");
        CHECK_EQ(i->realname(), cwd / "output_directory" / "1.txt");
    }
    {
        auto i = Input::make_p(123, cwd / "3.txt", cwd / "output_directory");
        CHECK_EQ(i->realname(), cwd / "output_directory" / "3.txt");
    }
    {
        auto i = Input::make_p(123, cwd / "absolute_directory" / "4.txt", cwd / "output_directory");
        CHECK_EQ(i->realname(), cwd / "output_directory" / "4.txt");
    }
}

TEST_CASE("TestBelow") {
    auto b = TestBelow::make_p(123,  20,   300);
    CHECK_EQ(b->t(), 123);
    CHECK_EQ(b->l(), 20);
    CHECK_EQ(b->c(), 300);

}

TEST_CASE("POC node p") {
    std::vector<Node_p> nodes;
    nodes.push_back(TestBelow::make_p(123,   1,     1));
    nodes.push_back(TestBelow::make_p(123,   1,    20));
    nodes.push_back(TestBelow::make_p(123,  20,   300));
    nodes.push_back(TestBelow::make_p(123,  20,  4000));
    int n = 0;
    for (const auto &p: nodes) {
        if (p) ++n;
    }
    CHECK_EQ(n, 4);
    n = 0;
    nodes.clear();
    nodes.push_back(TestBelow::make_p(123,   1,     1));
    nodes.push_back(TestBelow::make_p(123,   1,    20));
    {
        nodes.push_back(TestBelow::make_p(123,  20,   300));
        nodes.push_back(TestBelow::make_p(123,  20,  4000));
    }
    n = 0;
    for (const auto &p: nodes) {
        if (p) ++n;
    }
    CHECK_EQ(n, 4);
}

TEST_CASE("POC node w") {
    std::vector<Node_w> nodes;
    auto p = TestBelow::make_p(123,   1,     1);
    nodes.push_back(p);
    {
        auto q = TestBelow::make_p(123,  20,   300);
        nodes.push_back(q);
        int n = 0;
        for (const auto &p: nodes) {
            if (p.lock()) ++n;
        }
        CHECK_EQ(n, 2);
    }
    int n = 0;
    for (const auto &p: nodes) {
        if (p.lock()) ++n;
    }
    CHECK_EQ(n, 1);
}

TEST_CASE("1 node") {
    auto i = Input::make_p(123);
    std::vector<Node_p> owner;
    owner.push_back(TestBelow::make_p(123,   1,     1));
    owner.push_back(TestBelow::make_p(123,   1,    20));
    owner.push_back(TestBelow::make_p(123,  20,     1));
    owner.push_back(TestBelow::make_p(123,  20,    20));
    owner.push_back(TestBelow::make_p(123,  20,   300));
    owner.push_back(TestBelow::make_p(123,  20,  4000));
    owner.push_back(TestBelow::make_p(123, 300,     1));
    owner.push_back(TestBelow::make_p(123, 300,    20));
    owner.push_back(TestBelow::make_p(123, 300,   300));
    owner.push_back(TestBelow::make_p(123, 300,  4000));
    owner.push_back(TestBelow::make_p(123, 300, 50000));

    for (const auto &p: owner) {
        Parser::push_back(i, p);
    }
    int ll = 0;
    i->iterate([&ll](int i){
        ll += i;
    });
    CHECK_EQ(ll, 321);
    ll = 0;
    i->iterate([&ll](int i, bool &stop){
        ll += i;
        if (i>10) stop = true;
    });
    CHECK_EQ(ll, 21);
    int cc = 0;
    i->iterate(1, [&cc](const Node_p &p) {
        cc += p->c();
    });
    CHECK_EQ(cc, 21);
    cc = 0;
    i->iterate(2, [&cc](const Node_p &p) {
        cc += p->c();
    });
    CHECK_EQ(cc, 0);
    cc = 0;
    i->iterate(20, [&cc](const Node_p &p) {
        cc += p->c();
    });
    CHECK_EQ(cc, 4321);
    cc = 0;
    i->iterate(300, [&cc](const Node_p &p) {
        cc += p->c();
    });
    CHECK_EQ(cc, 54321);
    owner.pop_back();
    owner.pop_back();
    cc = 0;
    i->iterate(300, [&cc](const Node_p &p) {
        cc += p->c();
    });
    CHECK_EQ(cc, 321);
}

TEST_SUITE_END();

