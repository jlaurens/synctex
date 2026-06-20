/**
 * Parser_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include "Parser.hpp"

using namespace SyncTeXpp;

TEST_SUITE_BEGIN("Parser/Content");

TEST_CASE("POC") {
    CHECK_EQ(1, 1);
}

TEST_CASE("Content/Void") {
    SYNCTEX_COVERAGE_PUSH();
    auto manager_p = Manager::make_p("Content/Void.whatever", false);
    auto parser_p = Parser::make_p(TestVisitor::i9on_p(manager_p));
    int error_count = 0;
    auto status = TestVisitor::parse_content(parser_p, error_count);
    CHECK_EQ(error_count, 1);
    CHECK_EQ(status, Status::ErrorDataMissing);
    SYNCTEX_COVERAGE_POP();
}

#define SYNCTEX_TEST_BASIC_CASE(WHAT, IN_FORM, ERROR, STATUS)           \
TEST_CASE("Content/" WHAT) {                                             \
    SYNCTEX_COVERAGE_PUSH();                                              \
    auto manager_p = Manager::make_p("Content/" WHAT ".whatever", false);  \
    auto parser_p = Parser::make_p(TestVisitor::i9on_p(manager_p));         \
    int error_count = 0;                                                     \
    auto status = TestVisitor::parse_content(parser_p, error_count, IN_FORM); \
    if (error_count != ERROR)           \
        SYNCTEX_COVERAGE_PRINT("ERROR"); \
    CHECK_EQ(ERROR, error_count);         \
    CHECK_EQ(status, Status::STATUS);      \
    SYNCTEX_COVERAGE_POP();                 \
}

SYNCTEX_TEST_BASIC_CASE("sheet end", false, 0, Done);
SYNCTEX_TEST_BASIC_CASE("sheet end error", false, 1, Done);
SYNCTEX_TEST_BASIC_CASE("sheet end error(2)", false, 1, Done);

TEST_CASE("Content/insert_vbox") {
    auto manager_p = Manager::make_p("Content/VBox.whatever", false);
    auto parser_p = Parser::make_p(TestVisitor::i9on_p(manager_p));
    tlc_hv_WHD_s s = {1, 20, 300, 4000, 50000, 600000, 7000000, 80000000};
    TestVisitor::insert_vbox(parser_p, s);
    auto p = TestVisitor::above_p(parser_p);
    REQUIRE(p);
    CHECK_EQ(p->tlc_hv_WHD(), s);
}

SYNCTEX_TEST_BASIC_CASE("Not a box", false, 0, Done);
SYNCTEX_TEST_BASIC_CASE("form error 1", false, 2, Failed);
SYNCTEX_TEST_BASIC_CASE("form error 2", false, 2, Failed);
SYNCTEX_TEST_BASIC_CASE("form", false, 0, Done);


#define SYNCTEX_TEST_CASE(WHAT, CASE)                                  \
TEST_CASE("Content/" WHAT) {                                            \
    SYNCTEX_COVERAGE_PUSH();                                             \
    auto manager_p = Manager::make_p("Content/" WHAT ".whatever", false); \
    auto parser_p = Parser::make_p(TestVisitor::i9on_p(manager_p)); \
    int error_count = 0;                                             \
    auto status = TestVisitor::parse_content(parser_p, error_count);  \
    CHECK_EQ(error_count, 0);                \
    CHECK_EQ(status, Status::Done);           \
    if (CASE != 0) {                           \
       auto p = TestVisitor::below_p(parser_p); \
        REQUIRE(p);           \
        switch(CASE) {         \
            case 1:             \
            CHECK_EQ(p->t(), 1); \
            CHECK_EQ(p->l(), 20); \
            CHECK_EQ(p->c(), 300); \
            CHECK_EQ(p->h(), 4000); \
            CHECK_EQ(p->v(), 50000); \
            CHECK_EQ(p->W(), 600000); \
            CHECK_EQ(p->H(), 7000000); \
            CHECK_EQ(p->D(), 80000000); \
            break;                       \
            case 2:             \
            CHECK_EQ(p->t(), 1); \
            CHECK_EQ(p->l(), 20); \
            CHECK_EQ(p->c(), 300); \
            CHECK_EQ(p->h(), 4000); \
            CHECK_EQ(p->v(), 50000); \
            CHECK_EQ(p->W(), -600000);\
            CHECK_EQ(p->H(), 0);       \
            CHECK_EQ(p->D(), 0);        \
            break;                       \
            case 3:             \
            CHECK_EQ(p->t(), 1); \
            CHECK_EQ(p->l(), 20); \
            CHECK_EQ(p->c(), 300); \
            CHECK_EQ(p->h(), 4000); \
            CHECK_EQ(p->v(), 50000); \
            CHECK_EQ(p->W(), 0);      \
            CHECK_EQ(p->H(), 0);       \
            CHECK_EQ(p->D(), 0);        \
            break;                       \
            case 4:             \
            CHECK_EQ(p->t(), 1); \
            CHECK_EQ(p->l(), 0);  \
            CHECK_EQ(p->c(), 0);   \
            CHECK_EQ(p->h(), 4000); \
            CHECK_EQ(p->v(), 50000); \
            CHECK_EQ(p->W(), 0);      \
            CHECK_EQ(p->H(), 0);       \
            CHECK_EQ(p->D(), 0);        \
            break;                       \
            default:            \
            CHECK_EQ(p->t(), 0); \
            CHECK_EQ(p->l(), 0);  \
            CHECK_EQ(p->c(), 0);   \
            CHECK_EQ(p->h(), 0);    \
            CHECK_EQ(p->v(), 0);     \
            CHECK_EQ(p->W(), 0);      \
            CHECK_EQ(p->H(), 0);       \
            CHECK_EQ(p->D(), 0);        \
            break;                       \
        }                 \
    }                      \
    SYNCTEX_COVERAGE_POP(); \
}

SYNCTEX_TEST_CASE("comment", 0)
SYNCTEX_TEST_CASE("vbox", 1)
SYNCTEX_TEST_CASE("hbox", 1)
SYNCTEX_TEST_CASE("void vbox", 1)
SYNCTEX_TEST_CASE("void hbox", 1)
SYNCTEX_TEST_CASE("rule", 1)
SYNCTEX_TEST_CASE("kern", 2)
SYNCTEX_TEST_CASE("glue", 3)
SYNCTEX_TEST_CASE("math", 3)
SYNCTEX_TEST_CASE("bdry", 3)
SYNCTEX_TEST_CASE("form ref", 4)

TEST_CASE("form") {
    SYNCTEX_COVERAGE_PUSH();
    Tester::test_synctex(
        "form.whatever",
R"(<456
>456
}123
)",
        [](const std::string &fullpath) -> void {
            auto manager_p = Manager::make_p(fullpath, false);
            auto parser_p = Parser::make_p(TestVisitor::i9on_p(manager_p));
            int error_count = 0;
            auto status = TestVisitor::parse_content(parser_p, error_count, false);
            if (error_count != 0)
                SYNCTEX_COVERAGE_PRINT("ERROR COUNT");
            CHECK_EQ(error_count, 0);
            CHECK_EQ(status, Status::Done);
            CHECK(manager_p->form_p(456));
        }
    );
    SYNCTEX_COVERAGE_POP();
}

TEST_CASE("form with ref no below") {
    SYNCTEX_COVERAGE_PUSH();
    Tester::test_synctex(
        "form.whatever",
R"(<456
>456
f456:111,222
}123
)",
        [](const std::string &fullpath) -> void {
            auto manager_p = Manager::make_p(fullpath, false);
            auto parser_p = Parser::make_p(TestVisitor::i9on_p(manager_p));
            int error_count = 0;
            auto status = TestVisitor::parse_content(parser_p, error_count, false);
            CHECK_EQ(error_count, 1);
            CHECK_EQ(status, Status::ErrorDataBadRefTag);
            CHECK(manager_p->form_p(456));
        }
    );
    SYNCTEX_COVERAGE_POP();
}

TEST_CASE("form with ref and below") {
    SYNCTEX_COVERAGE_PUSH();
    Tester::test_synctex(
        "form.whatever",
R"(<456
h1,20,300:4000,50000:600000,7000000,80000000
>456
f456:111,222
}123
)",
        [](const std::string &fullpath) -> void {
            auto manager_p = Manager::make_p(fullpath, false);
            auto parser_p = Parser::make_p(TestVisitor::i9on_p(manager_p));
            int error_count = 0;
            auto status = TestVisitor::parse_content(parser_p, error_count, false);
            CHECK_EQ(error_count, 0);
            CHECK_EQ(status, Status::Done);
            CHECK(manager_p->form_p(456));
        }
    );
    SYNCTEX_COVERAGE_POP();
}

TEST_SUITE_END();
