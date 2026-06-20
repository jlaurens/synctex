/**
 * Input.hpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#ifndef SYNCTEX_TEST_INPUT_HPP
#define SYNCTEX_TEST_INPUT_HPP

#include "SyncTeXInput.hpp"

namespace SyncTeXpp {

class Parser {
    public:
    static void push_back(Input_p &i, const Node_p &p) {
        if (i) i->push_back(p);
    }
};

};

#endif // SYNCTEX_TEST_INPUT_HPP
