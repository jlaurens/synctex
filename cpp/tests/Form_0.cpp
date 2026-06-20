/**
 * Form_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It implements the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include <memory>

#include "Tree.hpp"
#include "Form.hpp"

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Form");

TEST_CASE("Tag")  {
    auto form = TestForm::make_p(321);
    CHECK_EQ(form->t(), 321);
    form->set_t(654);
    CHECK_EQ(form->t(), 654);
}

TEST_CASE("Form/Below")  {
    auto below = TestBelow::make_p(1, 20, 300);
    auto form = TestForm::make_p(4000);
    // Forbidden: below->set_above_p(above);
    // std::cout << "BELOW == " << below << std::endl;
    CHECK_EQ(below, form->set_below_p(below));
    CHECK_EQ(below, form->set_below_p(below));
    CHECK_EQ(below, form->below_p());
    CHECK_EQ(form, below->above_p());

    auto below2 = TestBelow::make_p(50000, 600000, 7000000);
    CHECK_EQ(below2, form->set_below_p(below2));
    CHECK_EQ(below2, form->below_p());
    CHECK_EQ(form, below2->above_p());
    CHECK_EQ(below2, below->left_p());
    CHECK_EQ(below, below2->right_p());

    auto form2 = TestForm::make_p();
    CHECK_EQ(nullptr, form2->set_below_p(below));
    CHECK_EQ(nullptr, form2->below_p());
    CHECK_EQ(form, below->above_p());
    
}

TEST_CASE("Form/Left/Right")  {
    auto form1 = TestForm::make_p();
    auto form2 = TestForm::make_p();

    auto below1 = TestBelow::make_p();
    auto below2 = TestBelow::make_p();
    auto below3 = TestBelow::make_p();

    CHECK_EQ(below1, form1->set_below_p(below1));
    SYNCTEX_CHECK_ALBRr(form1, nullptr, nullptr, below1, nullptr, below1);
    SYNCTEX_CHECK_ALR (below1, form1, nullptr, nullptr);

    CHECK_EQ(below2, below1->set_right_p(below2));
    SYNCTEX_CHECK_ALBRr(form1, nullptr, nullptr, below1, nullptr, below2);
    SYNCTEX_CHECK_ALR (below1, form1, nullptr, below2);
    SYNCTEX_CHECK_ALR (below2, form1, below1, nullptr);

    CHECK_EQ(below3, below2->set_right_p(below3));
    SYNCTEX_CHECK_ALBRr(form1, nullptr, nullptr, below1, nullptr, below3);
    SYNCTEX_CHECK_ALR (below1, form1, nullptr, below2);
    SYNCTEX_CHECK_ALR (below2, form1, below1, below3);
    SYNCTEX_CHECK_ALR (below3, form1, below2, nullptr);

}

TEST_CASE("this")  {
    auto form1 = TestForm::make_p();
    auto below1 = TestBelow::make_p();

    SYNCTEX_CHECK_ALBRr(form1, nullptr, nullptr, nullptr, nullptr, nullptr);
    SYNCTEX_CHECK_ALR (below1, nullptr, nullptr, nullptr);
  
    CHECK_EQ(nullptr, form1->set_below_p(form1));
    SYNCTEX_CHECK_ALBRr(form1, nullptr, nullptr, nullptr, nullptr, nullptr);
    SYNCTEX_CHECK_ALR (below1, nullptr, nullptr, nullptr);
 
    CHECK_EQ(nullptr, form1->set_right_p(form1));
    SYNCTEX_CHECK_ALBRr(form1, nullptr, nullptr, nullptr, nullptr, nullptr);
    SYNCTEX_CHECK_ALR (below1, nullptr, nullptr, nullptr);

    CHECK_EQ(nullptr, below1->set_right_p(below1));
    SYNCTEX_CHECK_ALBRr(form1, nullptr, nullptr, nullptr, nullptr, nullptr);
    SYNCTEX_CHECK_ALR (below1, nullptr, nullptr, nullptr);
}

TEST_SUITE_END();

