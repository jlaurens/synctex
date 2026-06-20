/**
 * Parser.hpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#ifndef SYNCTEX_TEST_PARSER_HPP
#define SYNCTEX_TEST_PARSER_HPP

#include "TestUtils.hpp"

#include "BufferChef.hpp"
#include "SyncTeXInput.hpp"
#include "SyncTeXForm.hpp"
#include "SyncTeXSheet.hpp"
#include "SyncTeXManager.hpp"
#include "SyncTeXReader.hpp"
#include "SyncTeXParser.hpp"

namespace SyncTeXpp {

class TestVisitor {
    public:

    static Status parse_content(
        Parser_p p,
        int &error_count,
        bool in_form=false,
        int t=123
    ) {
        if (p) {
            if (in_form) {
                auto form_p = Form::make_p(t+333);
                p->_form_p = form_p;
                p->_above_p = form_p;
                p->_i9on_p->_form_p_by_tag[t] = form_p;
            } else {
                auto sheet_p = Sheet::make_p(t);
                p->_sheet_p = sheet_p;
                p->_above_p = sheet_p;
                p->_i9on_p->_sheet_p_by_tag[t] = sheet_p;
                SYNCTEX_R_COVERAGE(std::string("sheet_p->t()==") + std::to_string(sheet_p->t()));
            }
            return p->parse_content(error_count);
        }
        return Status::Error;
    }
    static void insert_vbox(
        Parser_p p,
        tlc_hv_WHD_s s,
        int t=123,
        bool in_form=false
    ) {
        if (p) {
            if (in_form) {
                auto form_p = Form::make_p(t);
                p->_form_p = form_p;
                p->_above_p = form_p;
            } else {
                auto sheet_p = Sheet::make_p(t);
                p->_sheet_p = sheet_p;
                p->_above_p = sheet_p;
            }
            p->insert_vbox(s);
        }
    }
    static Manager::I9on_p i9on_p(Manager_p p) {
        return p->_i9on_p;
    }
    static Node_p above_p(Parser_p p) {
        return p->_above_p;
    }
    static Node_p below_p(Parser_p p) {
        return p->_below_p;
    }
};

};

#endif // SYNCTEX_TEST_PARSER_HPP
