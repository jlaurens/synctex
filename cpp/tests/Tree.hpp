/**
 * Tree.hpp
 * 
 * This file is part of the SyncTeX library.
 * It implements the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#ifndef SYNCTEX_TESTS_TREE_HPP
#define SYNCTEX_TESTS_TREE_HPP

#include "Node.hpp"
#include "SyncTeXTree.hpp"

namespace SyncTeXpp {

class TestBelow;
using TestBelow_p = std::shared_ptr<TestBelow>;
using TestBelow_w = std::weak_ptr<TestBelow>;

class TestAbove;
using TestAbove_p = std::shared_ptr<TestAbove>;
using TestAbove_w = std::weak_ptr<TestAbove>;

class TestBelow: public Below
{
    int _l = 0;
    int _c = 0;
    protected:
    TestBelow(int t, int l, int c): Below(t), _l(l), _c(c) {}
    public:
    static TestBelow_p make_p(int t=0, int l=0, int c=0) {
        return std::shared_ptr<TestBelow>(
            new TestBelow(t, l, c)
        );
    }
    int l() const override { return _l; }
    int c() const override { return _c; }

    std::ostream & log(std::ostream &os) const override {
        return os << 'B' << t() << "," << l() << "," << c();
    };

};

std::ostream& operator<<(std::ostream& os, const TestBelow_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

class TestAbove: public Above
{
    protected:
    TestAbove(int t=0): Above(t) {}
    public:
    static TestAbove_p make_p(int t = 0) {
        return std::shared_ptr<TestAbove>(
            new TestAbove(t)
        );
    }
    std::ostream & log(std::ostream &os) const override {
        return os << 'A' << t();
    };
};

std::ostream& operator<<(std::ostream& os, const TestAbove_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}


};

#endif // SYNCTEX_TESTS_TREE_HPP
