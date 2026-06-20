/**
 * Parser_1.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include "Parser.hpp"

using namespace SyncTeXpp;

TEST_SUITE_BEGIN("Parser/Preamble");

TEST_CASE("Preamble") {
    SYNCTEX_COVERAGE_PUSH();
    Tester::test_synctex(
        "1.whatever",
R"(SyncTeX version:19
Magnification:15
Unit:7
X Offset:12
Y Offset:130
!66
Content:
!13
Postamble:
!15
)",
        [](const std::string &fullpath) -> void {
            auto manager_p = Manager::make_p(fullpath, false);
            int error_count = 0;
            auto status = manager_p->parse(error_count);
            CHECK_EQ(manager_p->version(), 19);
            CHECK_EQ(manager_p->pre_magnification(), 15);
            CHECK_EQ(manager_p->pre_unit(), 7);
            CHECK_EQ(manager_p->pre_x_offset(), 12);
            CHECK_EQ(manager_p->pre_y_offset(), 130);

            if (error_count != 0)
                SYNCTEX_COVERAGE_PRINT("ERROR COUNT");
            CHECK_EQ(error_count, 0);
            CHECK_EQ(status, Status::Done);
        }
    );
    SYNCTEX_COVERAGE_POP();
}

TEST_CASE("Preamble") {
    SYNCTEX_COVERAGE_PUSH();
    Tester::test_synctex(
        "1.whatever",
R"(SyncTeX version:19
Magnification:15
Unit:7
X Offset:12
Y Offset:130
!66
Content:
!13
{1001
[8,80,800:8000,80000:800000,8000000,80000000
$9,90,900:9000,90000
]
}1001
Postamble:
!15
)",
        [](const std::string &fullpath) -> void {
            auto manager_p = Manager::make_p(fullpath, false);
            int error_count = 0;
            auto status = manager_p->parse(error_count);
            CHECK_EQ(manager_p->version(), 19);
            CHECK_EQ(manager_p->pre_magnification(), 15);
            CHECK_EQ(manager_p->pre_unit(), 7);
            CHECK_EQ(manager_p->pre_x_offset(), 12);
            CHECK_EQ(manager_p->pre_y_offset(), 130);
            if (error_count != 0)
                SYNCTEX_COVERAGE_PRINT("ERROR COUNT");
            CHECK_EQ(error_count, 0);
            CHECK_EQ(status, Status::Done);
            auto sheet_p = manager_p->sheet_p(1001);
            CHECK(sheet_p);
            auto b_p = sheet_p->below_p();
            CHECK(b_p);
            b_p = b_p->below_p();
            CHECK(b_p);
            b_p = b_p->right_p();
            CHECK(b_p);
            b_p = b_p->right_p();
            CHECK(b_p);
            CHECK(b_p->right_p() == nullptr);
        }
    );
    SYNCTEX_COVERAGE_POP();
}


TEST_SUITE_END();
