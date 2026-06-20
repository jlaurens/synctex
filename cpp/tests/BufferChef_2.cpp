/**
 * BufferChef_2.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */


#include <iostream>
#include <memory>
#include <limits>

#include "BufferChef.hpp"
 
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

using namespace SyncTeXpp;

TEST_SUITE("[BufferChef]") {

TEST_CASE("BufferChef() decode string") {
    auto chef = TestBufferChef(10,
        "0123\n56\n89"
        "abc\nefghij"
        "klmnopqrst"
        "uvxyzABCDE"
        "FGH\nJKL"
    );
    std::string line;
    CHECK(Status::Done == chef.decode(line));
    CHECK(line == "0123");
    CHECK(chef.test_string() == "56\n89");
    CHECK(Status::Done == chef.decode(line));
    CHECK(line == "012356");
    CHECK(chef.test_string() == "89");
    CHECK(Status::Done == chef.decode(line));
    CHECK(line == "01235689abc");
    CHECK(chef.test_string() == "efghij");
    CHECK(Status::Done == chef.decode(line));
    CHECK(line == "01235689abcefghijklmnopqrstuvxyzABCDEFGH");
    CHECK(chef.test_string() == "JKL");
    CHECK(Status::EndOfData == chef.decode(line));
    CHECK(line == "01235689abcefghijklmnopqrstuvxyzABCDEFGHJKL");
    CHECK(chef.test_string() == "⟨NONE⟩");
}

#define SYNCTEX_TEST(STR, VALUE)                    \
    chef.test_reset("abcdefghi:" STR "X");          \
    CHECK(Status::Done == chef.expect(1));          \
    CHECK(chef.advance(9) == 9);                    \
    CHECK(chef.test_string() == ":");               \
    value = 0;                                      \
    CHECK(Status::Done == chef.decode(value, ':')); \
    CHECK(value == VALUE);                          \
    CHECK(chef.test_string() == "X")


TEST_CASE("BufferChef() decode int") {
    auto chef = TestBufferChef(10,
        "00123ABCDE"
    );
    int value;
    CHECK(Status::Done == chef.decode(value));
    CHECK(value == 123);
    CHECK(chef.test_string() == "ABCDE");
    chef.test_reset("-0123ABCDE");
    CHECK(chef.capacity() == 10);
    CHECK(chef.size() == 0);
    CHECK(chef.position() == 0);
    value = 0;
    CHECK(Status::Done == chef.decode(value));
    CHECK(value == -123);
    CHECK(chef.test_string() == "ABCDE");
    chef.test_reset(":0123ABCDE");
    value = 0;
    CHECK(Status::Done == chef.decode(value, ':'));
    CHECK(value == 123);
    CHECK(chef.test_string() == "ABCDE");
    chef.test_reset(":-123ABCDE");
    value = 0;
    CHECK(Status::Done == chef.decode(value, ':'));
    CHECK(value == -123);
    CHECK(chef.test_string() == "ABCDE");
    chef.test_reset("012345678" ":-123ABCDE");
    chef.expect(1);
    chef.advance(9);
    value = 0;
    CHECK(Status::Done == chef.decode(value, ':'));
    CHECK(value == -123);
    CHECK(chef.test_string() == "ABCDE");

    SYNCTEX_TEST("4294967295", -1);
    SYNCTEX_TEST("-4294967295", 1);
    SYNCTEX_TEST( "429496729" "1705032705", 1);
    SYNCTEX_TEST("-429496729" "1705032705", -1);
}

TEST_CASE("BufferChef() decode int") {
    auto chef = TestBufferChef(10, "=ABCDE" );
    int value = 0;
    CHECK(Status::Done == chef.decode(value, 123));
    CHECK(value == 123);
    CHECK(chef.test_string() == "ABCDE");
    chef.test_reset(":=ABCDE");
    value = 0;
    CHECK(Status::Done == chef.decode(value, ':', 123));
    CHECK(value == 123);
    CHECK(chef.test_string() == "ABCDE");
}

TEST_CASE("BufferChef() decode united") {
    auto chef = TestBufferChef(10, "=ABCDE" );
    int value = 0;
    CHECK(Status::Done == chef.decode(value, 123));
    CHECK(value == 123);
    CHECK(chef.test_string() == "ABCDE");
    chef.test_reset(":=ABCDE");
    value = 0;
    CHECK(Status::Done == chef.decode(value, ':', 123));
    CHECK(value == 123);
    CHECK(chef.test_string() == "ABCDE");
}

#undef SYNCTEX_TEST
#define SYNCTEX_TEST(STR, VALUE, AFTER)             \
    chef.test_reset("abcdefghi:" STR AFTER);        \
    CHECK(Status::Done == chef.expect(1));          \
    CHECK(chef.advance(9) == 9);                    \
    CHECK(chef.test_string() == ":");               \
    value = 0;                                      \
    CHECK(Status::Done == chef.decode(value, ':')); \
    CHECK(value == doctest::Approx(VALUE).epsilon(0.00001)); \
    CHECK(chef.test_string() == AFTER)

    TEST_CASE("BufferChef() decode float") {
    auto chef = TestBufferChef(10, "");
    float value;
    SYNCTEX_TEST("012345", 12345, "X");
    SYNCTEX_TEST("-12345", -12345, "X");
    SYNCTEX_TEST("0.12345", 0.12345, "X");
    SYNCTEX_TEST(".12345", 0.12345, "X");
    SYNCTEX_TEST("-0.12345", -.12345, "X");
    SYNCTEX_TEST("123.45", 123.45, "X");
    SYNCTEX_TEST("0123.45", 123.45, "X");
    SYNCTEX_TEST("-.12345", -.12345, "X");
    SYNCTEX_TEST("-0123.45", -123.45, "X");

    SYNCTEX_TEST("0.12345", 0.12345, ".X");
    SYNCTEX_TEST("012.345", 12.345, ".X");
    SYNCTEX_TEST(".12345", 0.12345, ".X");
    SYNCTEX_TEST("12.345", 12.345, ".X");
    SYNCTEX_TEST("-0.12345", -0.12345, ".X");
    SYNCTEX_TEST("-012.345", -12.345, ".X");
    SYNCTEX_TEST("-.12345", -.12345, ".X");
    SYNCTEX_TEST("-12.345", -12.345, ".X");
}

#undef SYNCTEX_CHECK
#define SYNCTEX_CHECK(UNIT,VALUE)                        \
    chef.test_reset("1." UNIT "X");                       \
    value = 0;                                             \
    CHECK_EQ(Status::Done, chef.decode_united(value));      \
    CHECK_EQ(value, doctest::Approx(VALUE).epsilon(0.00001));\
    CHECK_EQ(chef.test_string(), "X")

TEST_CASE("BufferChef() decode united") {
    auto chef = TestBufferChef(10, "");
    float value;
    SYNCTEX_CHECK("sp", 1.0f        );   // 1.0f
    SYNCTEX_CHECK("pt", 65536.0f    );   // 65536.0f
    SYNCTEX_CHECK("bp", 65781.76f   );   // 72.27f / 72 * 65536.0
    SYNCTEX_CHECK("nd", 69925.4829f );   // 685.0f / 642 * 65536
    SYNCTEX_CHECK("dd", 70124.0864f );   // 1238.0f / 1157 * 65536.0
    SYNCTEX_CHECK("mm", 186467.9811f);   // 72.27f * 65536 / 25.4
    SYNCTEX_CHECK("pc", 786432.0f   );   // 12.0f * 65536.0
    SYNCTEX_CHECK("nc", 839105.7944f);   // 1370.0f / 107 * 65536
    SYNCTEX_CHECK("cc", 841489.0372f);   // 14856.0f / 1157 * 65536
    SYNCTEX_CHECK("cm", 1864679.811f);   // 72.27f * 65536 / 2.54
    SYNCTEX_CHECK("in", 4736286.72f );   // 72.27f * 65536
}

}

TEST_SUITE_END();

