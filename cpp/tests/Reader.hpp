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

#include "TestUtils.hpp"

#include "SyncTeXReader.hpp"

namespace SyncTeXpp {

class TestVisitor
{
    public:
    static std::string data(Reader_p r_p, int s = 0) {
        int size = 0;
        auto d = r_p->data(size);
        return std::string(d, s>0 ? std::min(s, size) : size);
    }

    static fs::path synctex(Reader_p r_p) {
        return r_p->_synctex;
    }

};

};
