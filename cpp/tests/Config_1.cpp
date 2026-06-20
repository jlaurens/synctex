/**
 * Config_1.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */


#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("filesystem") {
    std::string path = "A/B/C";
    CHECK(fs::path(path).string() == path);
    CHECK(fs::is_directory("../.."));
}
