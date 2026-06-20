/**
 * SyncTeXNodeHandle.hpp
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

#ifndef SYNCTEX_NODE_HANDLE_HPP
#define SYNCTEX_NODE_HANDLE_HPP

#include <memory>
#include "SyncTeXUtils.hpp"
#include "SyncTeXTree.hpp"

namespace SyncTeXpp {

class Handle;
using Handle_p = std::shared_ptr<Handle>;
using Handle_w = std::weak_ptr<Handle>;

class Handle: public std::enable_shared_from_this<Handle>
{
    Handle_w _above_p;
    Handle_p _below_p;
    Handle_w _left_p;
    Handle_p _right_p;
    Below_p _target_p;
    explicit Handle(Below_p target_p = nullptr): _target_p(target_p) {}
    private:
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
    Handle(Handle&&) = delete;
    Handle& operator=(Handle&&) = delete;
    public:
    static Handle_p make_p(Below_p target_p = nullptr)
    {
        return Handle_p( new Handle(target_p) );
    };
    Below_p target() const { return _target_p; }
    // Node_p set_target(Node_p p) {
    //     auto old_p = _target_p;
    //     if (old_p == p) { return p; }
    //     _target_p = p;
    //     return old_p;
    // }
    Handle_p above_p() const { return _above_p.lock(); }
    Handle_p set_below_p(Handle_p p) {
        auto old_p = _below_p;
        if (old_p == p) { return p; }
        if (p) {
            p->detached();
        }
        _below_p = p;
        if (old_p) {
            old_p->set_above_p(nullptr);
        }
        return old_p;
    }
    private:
    Handle_p detached() {
        auto ans = shared_from_this();
        auto left_p = _left_p.lock();
        if (left_p) {
            _left_p = Handle_w();
            if (left_p->_right_p == ans) {
                left_p->set_right_p(nullptr);
            }
        }
        auto above_p = _above_p.lock();
        if (above_p) {
            _above_p = Handle_w();
            if (above_p->_below_p == ans) {
                above_p->set_below_p(nullptr);
            }
        }
        return ans;
}
    Handle_p set_above_p(Handle_p p) {
        auto old_p = _above_p.lock();
        if (old_p == p) { return p; }
        _above_p = p;
        return old_p;
    }
    Handle_p below_p() const { return _below_p; }
    Handle_p left_p() const { return _left_p.lock(); }
    Handle_p set_left_p(Handle_p p) {
        auto old_p = _left_p.lock();
        if (old_p == p) { return p; }
        _left_p = p;
        return old_p;
    }
    Handle_p right_p() const { return _right_p; }
    Handle_p set_right_p(Handle_p p) {
        auto old_p = _right_p;
        if (old_p == p) { return p; }
        if (p) {
            p->detached();
        }
        _right_p = p;
        if (old_p) {
            old_p->set_left_p(nullptr);
        }
        return old_p;
    }


    Handle_p detached_below() {
        return _below_p ? _below_p->detached() : nullptr;
    }
    public:
    Handle_p make_above_p(Below_p target_p) {
        auto ans = Handle::make_p(target_p);
        ans->set_below_p(shared_from_this());
        return ans;
    }
};

/**
 * @brief The detached below of the reciver
 * 
 * @return a Handle_p
 */
inline Handle_p Handle::detached_below()
{}

/**
 *  When we have parsed a box, we must register
 *  all the contained heading boundary nodes
 *  that have not yet been registered.
 *  Those handles will be deleted when poping.
 */
static SYNCTEX_INLINE void _synctex_handle_make_friend_tlc(synctex_node_p node)
{
    while (node) {
        synctex_node_p target = _synctex_tree_reset_target(node);
        _synctex_node_make_friend_tlc(target);
        node = __synctex_tree_sibling(node);
    }
}

}; // namespace SyncTeXpp

#endif // SYNCTEX_NODE_HANDLE_HPP
