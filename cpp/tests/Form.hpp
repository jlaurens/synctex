/**
 * Form.hpp
 * 
 * This file is part of the SyncTeX library.
 * It implements the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#ifndef SYNCTEX_TESTS_FORM_HPP
#define SYNCTEX_TESTS_FORM_HPP

#include <memory>

#include "Node.hpp"
#include "SyncTeXForm.hpp"

namespace SyncTeXpp {

class TestForm;
using TestForm_p = std::shared_ptr<TestForm>;
using TestForm_w = std::weak_ptr<TestForm>;

class TestForm: public Form
{
    int _l = 0;
    protected:
    TestForm(int t=0, int l=0): Form(t), _l(l) {}
    public:
    static TestForm_p make_p(int t=0, int l=0) {
        return std::shared_ptr<TestForm>(
            new TestForm(t, l)
        );
    }
    int l() const override { return _l; }
};

};

#endif // SYNCTEX_TESTS_FORM_HPP
