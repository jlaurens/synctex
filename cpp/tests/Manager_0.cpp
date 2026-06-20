/**
 * Manager_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */


#include <iostream>

#include "Manager.hpp"

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Manager");

TEST_CASE("Constructor") {
    auto m_p = Manager::make_p("does not exist", "bar", true);
    auto parser_p = Parser::make_p(TestVisitor::i9on_p(m_p));
    int i;
    Parser::parse_sheet(TestVisitor::i9on_p(m_p), i);
    Parser::parse_form(TestVisitor::i9on_p(m_p), i);
    parser_p->parse_content(i);
    parser_p->parse_content(i);
    m_p->output_directory();
}

TEST_SUITE_END();
