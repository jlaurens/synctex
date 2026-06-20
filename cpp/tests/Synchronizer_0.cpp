/**
 * Synchronizer_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

#include "SyncTeX.hpp"

namespace SyncTeXpp {

class Node {
    int _t;
    public:
    Node(int t): _t(t) {}
    int t() { return _t; }
};

struct Scanner
{
    fs::path _output;
    fs::path _build_directory;
    bool _parse;
    Scanner(
        fs::path output,
        fs::path build_directory,
        bool parse
    ) {

    }
    bool foo(bool yorn) { return !yorn; } 
};

class SynchronizerI9on: public Scanner {
    public:
    SynchronizerI9on(
        std::string_view output,
        std::string_view build_directory,
        bool parse
    ): Scanner(
        fs::path(output),
        fs::path(build_directory),
        parse
    ) {}
};

Synchronizer::Synchronizer(
    std::string_view output,
    std::string_view build_directory,
    bool parse
) {
    _i9on_p = std::unique_ptr<SynchronizerI9on> ( new SynchronizerI9on(
        output,
        build_directory,
        parse
    ));
}

std::vector<Node_p> Synchronizer::edit(
    int page,
    float h,
    float v
) {
    return {
        std::make_shared<Node>(6),
        std::make_shared<Node>(4),
    };
}
std::vector<Node_p> Synchronizer::view(
    std::string_view name,
    int t,
    int l,
    int c
) {
    return {
        std::make_shared<Node>(66),
        std::make_shared<Node>(42),
    };
}
std::vector<Node_p> Synchronizer::view(
    std::string_view name,
    int t,
    int l,
    int c,
    std::function<bool(Node_p, Node_p)>
) {
    return {
        std::make_shared<Node>(666),
        std::make_shared<Node>(421),
    };
}

};

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Configure_0");

TEST_CASE("filesystem") {
    std::string path = "A/B/C";
    CHECK(fs::path(path).string() == path);
    CHECK(fs::is_directory("../.."));
}

TEST_CASE ("1==1") {
    CHECK(1==1);
}

TEST_CASE("Synchronizer_0")  {
    auto s = new SyncTeXpp::Synchronizer("FOO", "BAR", false);
    CHECK(1==1);
    auto ans = s->edit(1, 2, 3);
    CHECK(ans.size()==2);
    CHECK(ans[0]->t()==6);
    CHECK(ans[1]->t()==4);
    ans = s->view("foo", 4, 5, 6);
    CHECK(ans.size()==2);
    CHECK(ans[0]->t()==66);
    CHECK(ans[1]->t()==42);
    ans = s->view("foo", 7, 8, 9, [](SyncTeXpp::Node_p l, SyncTeXpp::Node_p r){ return l->t() < r->t(); });
    CHECK(ans.size()==2);
    CHECK(ans[1]->t()==421);
    CHECK(ans[0]->t()==666);
}

TEST_SUITE_END();


