/**
 * Manager_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#ifndef SYNCTEX_TEST_MANAGER_HPP
#define SYNCTEX_TEST_MANAGER_HPP

#include "TestUtils.hpp"
#include "SyncTeXManager.hpp"
#include "SyncTeXManagerI9on.hpp"

namespace SyncTeXpp {

class Parser {
    public:
    static std::shared_ptr<Parser> make_p(Manager::I9on_p) {
        return std::make_shared<Parser>();
    }
    static Status parse_form(Manager::I9on_p p, int&) {
        return Status::Done;
    }
    static Status parse_sheet(Manager::I9on_p p, int&) {
        return Status::Done;
    }
    Status parse_content(int&) {
        return Status::Done;
    }
};

class TestVisitor
{
    public:
    static std::string data(Reader_p r_p, int s = 0) {
        int size = 0;
        auto d = r_p->data(size);
        return std::string(d, s>0 ? std::min(s, size) : size);
    }

    static Manager::I9on_p i9on_p(Manager_p p) {
        return p->_i9on_p;
    }

};

};

#endif // SYNCTEX_TEST_MANAGER_HPP
