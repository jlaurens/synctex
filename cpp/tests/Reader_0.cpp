/**
 * Reader_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */


#include <iostream>
// #include <filesystem>
// #include <string>
// #include <zlib.h>

#include "Reader.hpp"

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("zlib");

TEST_CASE("filesystem") {
    std::string path = "A/B/C";
    CHECK_EQ(fs::path(path).string(), path);
    CHECK(fs::is_directory("../.."));
}

TEST_CASE("workdir") {
    std::string path = "0-READONLY.txt";
    auto p = fs::canonical(path);
    CHECK(fs::is_regular_file(p));
    gzFile file = gzopen(p.c_str(), "rb");
    CHECK(!!file);
    char buffer[1024];
    int bytesRead = gzread(file, buffer, sizeof(buffer));
    CHECK(bytesRead>=11);
    std::string actual(buffer, 11); // Convertir en std::string
    std::string expected = "DO NOT EDIT";
    CHECK_EQ(actual, expected);
    gzclose(file);
}

TEST_CASE("Reader") {
    CHECK_EQ(1, 1);
}

TEST_CASE("Reader") {
    std::string path = "0.txt";
    auto reader_p = Reader::make_p(path);
    CHECK(reader_p);
    reader_p->reset(2<<12);
    CHECK_EQ(reader_p->capacity(), 2<<12);
    std::string s = "SyncTeX Version:1\n"
                    "Input:1:/[...]]/./sync.tex\n";
    auto status = reader_p->expect(s.length());
    CHECK_EQ(Status::Done, status);
    CHECK_EQ(TestVisitor::data(reader_p, 100),
        "require_line => _current += 30"
        "\nSyncTeX Version:1"
        "\nInput:1:/Volumes/Users/Documents/synctex/synctex te"
    );
}

TEST_CASE("Reader unquoted/1") {
    fs::path output("with spaces but no quotes.any extentions");
    std::string jobname = output.filename().stem();
    auto reader_p = Reader::make_p(output);
    CHECK(reader_p);
    CHECK_EQ(TestVisitor::synctex(reader_p), "with spaces but no quotes.synctex");
    reader_p->reset(2<<12);
    CHECK_EQ(reader_p->capacity(), 2<<12);
    std::string s = "SyncTeX Version:1\n"
                    "Input:1:/[...]]/./sync.tex\n";
    auto status = reader_p->expect(s.length());
    CHECK_EQ(Status::Done, status);
    CHECK_EQ(TestVisitor::data(reader_p, 100),
        "require_line => _current += 30"
        "\nSyncTeX Version:1"
        "\nInput:1:/Volumes/Users/Documents/synctex/synctex te"
    );
}

TEST_CASE("Reader quoted") {
    fs::path output("with spaces and quotes.any extentions");
    std::string jobname = output.filename().stem();
    auto reader_p = Reader::make_p(output);
    CHECK(reader_p);
    CHECK_EQ(TestVisitor::synctex(reader_p), "with spaces and quotes.synctex");
    reader_p->reset(2<<12);
    CHECK_EQ(reader_p->capacity(), 2<<12);
    std::string s = "SyncTeX Version:1\n"
                    "Input:1:/[...]]/./sync.tex\n";
    auto status = reader_p->expect(s.length());
    CHECK_EQ(Status::Done, status);
    fs::rename("with spaces and quotes.synctex", "\"with spaces and quotes\".synctex");
    CHECK(fs::is_regular_file("\"with spaces and quotes\".synctex"));
    CHECK(!fs::is_regular_file("with spaces and quotes.synctex"));
}

TEST_CASE("Reader build dir") {
    std::string path = "1.any extentions";
    auto reader_p = Reader::make_p(path);
    CHECK(reader_p->atEnd());
    std::string build_dir = "foo_dir";
    CHECK(!fs::is_directory(build_dir));
    reader_p = Reader::make_p(path, build_dir);
    CHECK_EQ(reader_p->expect(1), Status::Failed);
    build_dir = "build_dir";
    CHECK(fs::is_directory(build_dir));
    reader_p = Reader::make_p(path, build_dir);
    CHECK_EQ(reader_p->expect(1), Status::Done);
}

TEST_CASE("Reader no file") {
    std::string path = "no file at all.any extentions";
    CHECK(!fs::is_regular_file(path));
    auto reader_p = Reader::make_p(path);
    CHECK_EQ(TestVisitor::synctex(reader_p), "");
    CHECK(reader_p->atEnd());
    CHECK_EQ(reader_p->expect(1), Status::Failed);
}

TEST_SUITE_END();
