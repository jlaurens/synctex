/**
 * SyncTeXNode.hpp
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

#ifndef SYNCTEX_NODE_HPP
#define SYNCTEX_NODE_HPP

#include <memory>
#include <tuple>
#include <string>
#include <ostream>

#include "SyncTeXUtils.hpp"

namespace SyncTeXpp {

class Node;

/**
 * @brief Shared pointer to a Node structure.
 * 
 */
using Node_p = std::shared_ptr<Node>;

/**
 * @brief Nodes for a readonly tree.
 * 
 * 
 */
class Node: public std::enable_shared_from_this<Node>
{
    /**
     * @brief This base class declares the shared behavior of all nodes.
     * 
     * All the other nodes are descendent from this class.
     */
    public:
    enum class Type { 
        Node = 0,
        Form,
        Sheet,
        HBox,
        VBox,
        VoidHBox,
        VoidVBox,
        Kern,
        Glue,
        Rule,
        Math,
        Bdry,
        BoxBdry,
        Ref,
        Proxy,
        ProxyLast,
        ProxyVBox,
        ProxyHBox,
    };
    /**
     * @brief The type of the node.
     * 
     * Mainly for introspection.
     * @return 
     */
    virtual Type type() const;
    virtual std::string type_name() const;

protected:
    Node() = default;
    public:
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(Node&&) = delete;
    Node& operator=(Node&&) = delete;

public:
    virtual ~Node() = default;  // Destructeur virtuel
    
    /**
     * @brief Return the receiver as Node_p pointer.
     * 
     * @return 
     */
    Node_p as_Node_p();

    /**
     * @brief The above node the receiver.
     * 
     * It is also the owner if there is no left node.
     * The default implementation returns `nullptr`.
     * 
     * @return a shared pointer to the above node.
     */
    virtual Node_p above_p() const;
    
    /**
     * @brief The below node the receiver.
     * 
     * The receiver owns its below node.
     * The default implementation returns `nullptr`.
     * 
     * @return a shared pointer to the below node.
     */
    virtual Node_p below_p() const;
    
    /**
     * @brief The rightmost node just below the receiver.
     * 
     * The default implementation returns `nullptr`.
     * 
     * @return a shared pointer to the rightmost node just below the receiver.
     */
    virtual Node_p rbelow_p() const;
    
    /**
     *  The rightmost of the receiver, possibly the receiver itself.
     * 
     * @return a shared pointer to the node rightmost to the receiver.
     */
    Node_p rightmost_p();
    
    /**
     * @brief The left node the receiver.
     * 
     * The receiver is ownd by its left node, if any,
     * otherwise it is owned by its above node, if any,
     * otherwise it is owned elsewhere.
     * The default implementation returns `nullptr`.
     * 
     * @return a shared pointer to the left node.
     */
    virtual Node_p left_p() const;
    
    /**
     * @brief The right node the receiver.
     * 
     * The receiver owns its right node, if any.
     * The default implementation returns `nullptr`.
     * 
     * @return a `std::shared_ptr` to the right node.
     */
    virtual Node_p right_p() const;
    
    /**
     * @brief The level of the receiver.
     * 
     * The default implementation returns 0.
     * It corresponds to the depth of the node in a tree hierarchy.
     * 
     * @return the level.
     */
    virtual int level() const;
    
    /**
     * @brief The tag of the receiver.
     * 
     * The default implementation returns 0.
     * 
     * @return the tag.
     */
    virtual int t() const;
    
    /**
     * @brief The line of the receiver.
     * 
     * The default implementation returns 0.
     * 
     * @return the line.
     */
    virtual int l() const;
    
    /**
     * @brief The column of the receiver.
     * 
     * The default implementation returns 0.
     * 
     * @return the column.
     */
    virtual int c() const;
    
    /**
     * @brief The h of the receiver.
     * 
     * The default implementation returns 0.
     * 
     * @return the h.
     */
    virtual int h() const;
    
    /**
     * @brief The v of the receiver.
     * 
     * The default implementation returns 0.
     * 
     * @return the v.
     */
    virtual int v() const;
    
    /**
     * @brief The width of the receiver.
     * 
     * The default implementation returns 0.
     * 
     * @return the width.
     */
    virtual int W() const;
    
    /**
     * @brief The heiGht of the receiver.
     * 
     * The default implementation returns 0.
     * 
     * @return the height.
     */
    virtual int H() const;
    
    /**
     * @brief The depth of the receiver.
     * 
     * The default implementation returns 0.
     * 
     * @return the depth.
     */
    virtual int D() const;
    
    /**
     * @brief The limits of the receiver.
     * 
     * @return the limits.
     */
    virtual wnes_s wnes() const;
    
    /**
     * @brief The visible limits of the receiver.
     * 
     * @return the limits.
     */
    virtual wnes_s wnes_V() const;
    
    /**
     * @brief The h limits of the receiver.
     * 
     * @return the h limits.
     */
    virtual we_s we() const;
    
    /**
     * @brief The v limits of the receiver.
     * 
     * @return the v limits.
     */
    virtual ns_s ns() const;
    
    /**
     * @brief The west limit of the receiver.
     * 
     * @return the west.
     */
    virtual int w() const;
    
    /**
     * @brief The north limit of the receiver.
     * 
     * @return the north.
     */
    virtual int n() const;
    
    /**
     * @brief The east limit of the receiver.
     * 
     * @return the east.
     */
    virtual int e() const;
    
    /**
     * @brief The south limit of the receiver.
     * 
     * @return the south.
     */
    virtual int s() const;
    
    /**
     * @brief The visible west limit of the receiver.
     * 
     * @return the west.
     */
    virtual int w_V() const;
    
    /**
     * @brief The visible north limit of the receiver.
     * 
     * @return the north.
     */
    virtual int n_V() const;
    
    /**
     * @brief The visible east limit of the receiver.
     * 
     * @return the east.
     */
    virtual int e_V() const;
    
    /**
     * @brief The visible south limit of the receiver.
     * 
     * @return the south.
     */
    virtual int s_V() const;
    
    /**
     * @brief The t/l/c combo of the receiver.
     * 
     * The default implementation returns 0.
     * 
     * @return the tag, line and column.
     */
    tlc_s tlc() const;
    
    /**
     * @brief The h/v combo of the receiver.
     * 
     * The default implementation returns 0.
     * 
     * @return the h and v.
     */
    hv_s hv() const;
    
    /**
     * @brief The tlc/h/v combo of the receiver.
     * 
     * The default implementation returns 0.
     * 
     * @return the tlc, h and v.
     */
    tlc_hv_s tlc_hv() const;

    /**
     * @brief The tlc/h/v/WHD combo of the receiver.
     * 
     * The default implementation returns 0.
     * 
     * @return the tlc, hh and and W, H, D.
     */
    tlc_hv_WHD_s tlc_hv_WHD() const;

    /**
     * @brief Deep first navigation.
     * 
     * @return the next node
     */
    Node_p next_p() const;

    /**
     * @brief Whether the receiver is a box.
     * 
     * @return true
     * @return false 
     */
    virtual bool isBox() const;

    /**
     * @brief Whether the receiver is a hbox.
     * 
     * @return true
     * @return false 
     */
    virtual bool isHBox() const;

    /**
     * @brief Whether the receiver is a vbox.
     * 
     * @return true
     * @return false 
     */
    virtual bool isVBox() const;

    /**
     * @brief Whether the receiver is a below node.
     * 
     * @return true
     * @return false 
     */
    virtual bool is_below() const;

    /**
     * @brief Whether the receiver is an above node.
     * 
     * @return true
     * @return false 
     */
    virtual bool is_above() const;

    /**
     * @brief Whether the receiver is a form node.
     * 
     * @return true
     * @return false 
     */
    virtual bool isForm() const;

    /**
     * @brief Whether the receiver is a sheet node.
     * 
     * @return true
     * @return false
     */
    virtual bool isSheet() const;

    virtual std::ostream & log(std::ostream &o) const;

    friend std::ostream& operator<<(std::ostream& os, const Node& a);

    virtual int distance_to(const hv_s &hv) const;
};

std::ostream& operator<<(std::ostream& os, const Node& a);

std::ostream& operator<<(std::ostream& os, const Node_p& p);

}; // namespace SyncTeXpp



#endif // SYNCTEX_NODE_HPP

