/**
 * SyncTeXsheet.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

// #pragma once

/**
 * @brief Nodes to represent SyncTeX data.
 * 
 * Used by a Scanner instance.
 */

#ifndef SYNCTEX_NODE_SHEET_HPP
#define SYNCTEX_NODE_SHEET_HPP

#include <memory>
#include <string>
#include "SyncTeXTree.hpp"
#include "SyncTeXPrefix.hpp"
#include "SyncTeXBox.hpp"

namespace SyncTeXpp {

class Sheet;
using Sheet_p = std::shared_ptr<Sheet>;

class Ref;
using Ref_p = std::shared_ptr<Ref>;

class Sheet: public Above
{
    std::vector<HBox_p> _hbox_p_v;

    public:
    Type type() const final override { return Type::Sheet; }
    std::string type_name() const final override { return "Sheet"; }
    char prefix() const final override { return Prefix::BEGIN_SHEET; }
    
    Sheet(const Sheet&) = delete;
    Sheet& operator=(const Sheet&) = delete;
    Sheet(Sheet&&) = delete;
    Sheet& operator=(Sheet&&) = delete;
    Sheet() = delete;
    
    protected:
    Sheet(
        int t = 0
    ): Above(t) {}
    public:
    static Sheet_p make_p(
        int t = 0
    ) {
        return Sheet_p(new Sheet(t));
    };
    
    int level() const override final { return -1; }
 
    bool detached() const { return false; }

    Sheet_p above_sheet_p() override final {
        return std::dynamic_pointer_cast<Sheet>(as_Node_p());
    }

    Box_p below_box() {
        return std::dynamic_pointer_cast<Box>(below_p());
    }

    HBox_p push_back(Above_p p) {
        if (p) {
            auto hbox_p = p->asHBox_p();
            if (hbox_p) _hbox_p_v.push_back(p->asHBox_p());
            return hbox_p;
        }
        return nullptr;
    }

    void foreach_hbox(std::function<bool(HBox_p)> f)
    {
        for (const auto &p: _hbox_p_v) {
            if (!f(p)) break;
        }
    }
    std::ostream & log(std::ostream &o) const override {
        o << "{" << std::endl;
        if (_below_p) {
            o << _below_p << std::endl;
        }
        o << "}" << _t;
        return o;
    }


};

inline std::ostream& operator<<(std::ostream& os, const Sheet_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

}; // namespace SyncTeXpp

#endif // SYNCTEX_NODE_SHEET_HPP

