/**
 * SyncTeXGeo.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the Node class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

// #pragma once

/**
 * @brief Nodes to represent SyncTeX geometrical features.
 * 
 * A subclass of Node with mutability.
 * Used by a Scanner instance.
 *
 * */

#ifndef SYNCTEX_GEO_HPP
#define SYNCTEX_GEO_HPP

#include "SyncTeXUtils.hpp"
#include "SyncTeXNode.hpp"

namespace SyncTeXpp {

class Below;
using Below_p = std::shared_ptr<Below>;

class Above;
using Above_p = std::shared_ptr<Above>;
using Above_w = std::weak_ptr<Above>;

class Geo: virtual public Node
{
public:
    Geo(const Geo&) = delete;
    Geo& operator=(const Geo&) = delete;
    Geo(Geo&&) = delete;
    Geo& operator=(Geo&&) = delete;

    Geo() = default;
    virtual ~Geo() = default;

    /**
     * @brief The distance between a wnes box and a hv is special.
     *
     *  It is a L1 distance, not the euclidian distance.
     *  We have to take into account the particular layout,
     *  and the box hierarchy.
     *  Given a box, there are 9 regions delimited by the lines of the edges of the box.
     *  The origin is at the top left corner of the page.
     *
     *      w   <   e
     *    1 │   2   │ 3
     * n ───╆━━━━━━━╅─── -5
     * ^  4 ┃   5   ┃ 6
     * s ───╄━━━━━━━╃─── +15
     *    7 │   8   │ 9
     *     -10     +20
     *
     *  In each region, there is a different formula.
     *
     * @param wnes 
     * @param hv 
     * @return int
     */
    static int h_distance_to(
        const wnes_s &wnes,
        const hv_s &hv
    ) {
        if (hv.h < wnes.w) { // regions 1, 4, 7
            return wnes.w - hv.h;
        } else if (hv.h > wnes.e) { // regions 3, 6, 9
            return hv.h - wnes.e;
        }
        return 0; // regions 2, 5, 8
    }
    /**
     * @brief Horizontal position of wnes relative to hv.
     * 
     * Negative when hv is to the right of wnes
     * or wnes is to the left of hv.
     * Positive when hv is to the left of wnes
     * or wnes is to the right of hv.
     * 
     * @param wnes 
     * @param hv 
     * @return 
     */
    static int h_relative_to(
        const wnes_s &wnes,
        const hv_s &hv
    ) {
        if (hv.h < wnes.w) { // regions 1, 4, 7, positive
            return wnes.w - hv.h;
        } else if (hv.h > wnes.e) { // regions 3, 6, 9, negative
            return wnes.e - hv.h;
        }
        return 0; // regions 2, 5, 8
    }
    static int v_distance_to(
        const wnes_s &wnes,
        const hv_s &hv
    ) {
        if (hv.v < wnes.n) { // regions 1, 2, 3
            return wnes.n - hv.v;
        } else if (hv.v > wnes.s) { // regions 7, 8, 9
            return hv.v - wnes.s;
        }
        return 0; // regions 4, 5, 6
    }
    /**
     * @brief Vertical position of wnes relative to hv
     * 
     * Positive when wnes is below hv
     * or hv is above wnes.
     * Negative when wnes is above hv
     * or hv is below wnes.
     * 
     * @param wnes 
     * @param hv 
     * @return int 
     */
    static int v_relative_to(
        const wnes_s &wnes,
        const hv_s &hv
    ) {
        if (hv.v < wnes.n) { // regions 1, 2, 3, positive
            return wnes.n - hv.v;
        } else if (hv.v > wnes.s) { // regions 7, 8, 9, negative
            return wnes.s - hv.v;
        }
        return 0; // regions 4, 5, 6
    }
    static int distance_to(
        const wnes_s &wnes,
        const hv_s &hv
    ) {
        return  h_distance_to(wnes, hv)
            +   v_distance_to(wnes, hv);
    }

    static hv_s relative_to(
        const wnes_s &wnes,
        const hv_s &hv
    ) {
        return {    h_relative_to(wnes, hv),
                    v_relative_to(wnes, hv) };
    }
    
    /** Rougly speaking, this is:
     *  node's h coordinate - hit point's h coordinate.
     *  If node is to the right of the hit point, then this distance is positive,
     *  if node is to the left of the hit point, this distance is negative.
     *  If the argument is a pdf form reference, then the child is used and returned instead.
     *  Last revision: Mon Apr 24 07:05:27 UTC 2017
     */
    // virtual int h_distance_to(const hv_s &hv) const {
    //     return h_distance_to(box(), hv);
    // }
    // virtual int h_relative_to(const hv_s &hv) const {
    //     return h_relative_to(box(), hv);
    // }
    virtual int h_distance_to(const hv_s &hv) const {
        auto we = this->we();
        if (we.w < hv.h) {
            return hv.h - we.w;
        }
        if (hv.h < we.e) {
            return we.e - hv.h;
        }
        return 0;
    }
    virtual int h_relative_to(const hv_s &hv) const {
        auto we = this->we();
        if (we.w < hv.h) {
            return we.w - hv.h;
        }
        if (hv.h < we.e) {
            return we.e - hv.h;
        }
        return 0;
    }
    virtual int v_distance_to(const hv_s &hv) const {
        auto ns = this->ns();
        if (ns.n < hv.v) {
            return hv.v - ns.n;
        }
        if (hv.v < ns.s) {
            return ns.s - hv.v;
        }
        return 0;
    }
    virtual int v_relative_to(const hv_s &hv) const {
        auto ns = this->ns();
        if (ns.n < hv.v) {
            return ns.n - hv.v;
        }
        if (hv.v < ns.s) {
            return ns.s - hv.v;
        }
        return 0;
    }
    int distance_to(const hv_s &hv) const override {
        return h_distance_to(hv) + v_distance_to(hv);
    }
    virtual hv_s relative_to(const hv_s &hv) const {
        return { h_relative_to(hv), v_relative_to(hv) };
    }

    virtual void setup_visible() {};    
    
    /**
     * @brief Whether the receiver contains the hit point
     * 
     * The default implementation always returns false.
     * 
     * @param hv 
     * @return true 
     * @return false 
     */
    virtual bool contains(const hv_s &hv) const { return false; };

        /**
     * @brief Whether the receiver contains the hit point
     * 
     * The default implementation always returns false.
     * 
     * @param hv 
     * @return true 
     * @return false 
     */
    virtual bool contains_V(const hv_s &hv) const { return false; };

};

};

#endif // SYNCTEX_GEO_HPP

#if 0
Next does not belong here.
    /**
     * @brief Undocumented
     * 
     *  The returned proxy will be the child or a sibling of owning_proxy.
     *  The returned proxy has no above, nor below node.
     *  Used only by below and right methods
     *  to create proxies on the fly.
     *  If the target already has a computed right,
     *  then the returned proxy has itself a right
     *  pointing to that already computed right.
     * 
     * @param to_p the target
     * @return 
     */
    virtual Node_p new_below_proxy_to(Node_p to_p) { return nullptr; };
    
    /**
     * @brief Undocumented
     * 
     *  Given a target node, create a list of proxies.
     *  The first proxy points to the target node,
     *  its sibling points to the target's sibling and so on.
     *  Returns the first created proxy, the last one and
     *  an error status.
     * 
     * @param to_p 
     * @return nnr_s 
     */
    virtual nnr_s new_below_proxies_to(Node_p to_p);
#endif
