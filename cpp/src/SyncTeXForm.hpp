/**
 * SyncTeXForm.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

// #pragma once
#ifndef SYNCTEX_FORM_HPP
#define SYNCTEX_FORM_HPP

/**
 * @brief Nodes to represent SyncTeX data.
 * 
 * Used by a Scanner instance.
 */

#include <memory>
#include <string>
#include "SyncTeXTree.hpp"

namespace SyncTeXpp
{

class Form;
using Form_p = std::shared_ptr<Form>;

class Ref;
using Ref_p = std::shared_ptr<Ref>;

class Form:
    public Above
{
    public:
    Type type() const override final { return Type::Form; }
    std::string type_name() const override final { return "Form"; }
    public:
    Form(const Form&) = delete;
    Form& operator=(const Form&) = delete;
    Form(Form&&) = delete;
    Form& operator=(Form&&) = delete;
    ~Form() = default;

    protected:
    explicit Form( int t = 0 ): Above(t) {}
    
    public:
    static Form_p make_p(int t = 0) {
        return Form_p(new Form(t));
    }

    Form_p above_form_p() override {
        auto p = shared_from_this();
        while (p && p->type() != Type::Form) {
            p = p->above_p();
        }
        /*  exit the while loop either when N_p is nullptr or N_p is a form */
        return std::dynamic_pointer_cast<Form>(p);
    }

};

}; // namespace SyncTeXpp

#endif // SYNCTEX_FORM_HPP

