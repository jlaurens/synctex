/**
 * BufferChef_3.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <iterator>


namespace fs = std::filesystem;

#include "BufferChef.hpp"

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("filesystem") {
    std::string path = "A/B/C";
    CHECK(fs::path(path).string() == path);
    CHECK(fs::is_directory("../.."));
}

TEST_SUITE("[BufferChef]") {

#define SYNCTEX_CHECK_DONE(WHAT) CHECK_EQ(Status::Done, WHAT)
TEST_CASE("BufferChef() synctex") {
    TestBufferChef chef(10, ".......SyncTeX Version:1\nblablabla");
    SYNCTEX_CHECK_DONE(chef.expect(1));
    CHECK_EQ(chef.advance(7), 7);
    CHECK_EQ(chef.test_string(0), "Syn");
    SYNCTEX_CHECK_DONE(chef.require_head("SyncTeX Version"));

}

TEST_CASE("BufferChef() synctex") {
    std::string path = "0-READONLY.txt";
    auto p = fs::canonical(path);
    CHECK(fs::is_regular_file(p));
    std::ifstream file(p);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + p.string());
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    // std::cout << "content <<<" << std::endl;
    // std::cout << content << std::endl;
    // std::cout << ">>> content" << std::endl;
    TestBufferChef chef(10, content);
    SYNCTEX_CHECK_DONE(chef.require_endl());
    CHECK_EQ(chef.test_string(), "Syn");
    SYNCTEX_CHECK_DONE(chef.require_head("SyncTeX Version"));
    int i = 0;
    SYNCTEX_CHECK_DONE(chef.decode(i));
    CHECK_EQ(1, i);
    SYNCTEX_CHECK_DONE(chef.require_endl());
    SYNCTEX_CHECK_DONE(chef.require_head("Input"));
    i = 0;
    SYNCTEX_CHECK_DONE(chef.decode(i));
    CHECK_EQ(1, i);
    std::string s;
    SYNCTEX_CHECK_DONE(chef.decode(s, ':'));
    CHECK_EQ(s, "/√olumes/ºsers/∂ocuments/æ/◊ery/¬ong/πath/µore/†han/„0/ºTF8/©haracters.tex");
    SYNCTEX_CHECK_DONE(chef.require_head("Content", true));
    SYNCTEX_CHECK_DONE(chef.decode(i, '!'));
    CHECK_EQ(2, i);
    SYNCTEX_CHECK_DONE(chef.require_endl());
    tlc_hv_s t5;
    SYNCTEX_CHECK_DONE(chef.decode_tlc_hv(t5, 'x'));
    CHECK_EQ(t5, tlc_hv_s(3, 4, 0, 5, 6));
    tlc_hv_W_s t6;
    SYNCTEX_CHECK_DONE(chef.decode_tlc_hv_W(t6, 'k'));
    CHECK_EQ(t6, tlc_hv_W_s(7, 8, 0, 9, 10, 11));
    tlc_hv_WHD_s t8;
    SYNCTEX_CHECK_DONE(chef.decode_tlc_hv_WHD(t8, '['));
    CHECK_EQ(t8, tlc_hv_WHD_s(12, 13, 0, 14, 15, 16, 17, 18));
    SYNCTEX_CHECK_DONE(chef.read_char(']', true));
    CHECK_EQ(chef.test_string(), "⟨NONE⟩");
};

}

TEST_SUITE_END();
