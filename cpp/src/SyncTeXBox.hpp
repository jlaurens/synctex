/**
 * SyncTeXBox.hpp
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
 * 
 * Class hierarchy:
 * 
 * Above_lc_hv_WHD
 * │ Geo
 * └─┴─ Box
 *      ├─ VBox
 *      └─ HBox
 * 
 */

#ifndef SYNCTEX_BOX_HPP
#define SYNCTEX_BOX_HPP

#include "SyncTeXPrefix.hpp"
#include "SyncTeXGeo.hpp"
#include "SyncTeXNoBox.hpp"
#include "SyncTeX_tlc_hv_WHD.hpp"

namespace SyncTeXpp {

class Box:
    public Above_lc_hv_WHD,
    public Geo
{
    protected:
    int _level = 0;

    public:
    Box(const Box&) = delete;
    Box& operator=(const Box&) = delete;
    Box(Box&&) = delete;
    Box& operator=(Box&&) = delete;
    ~Box() = default;

    protected:
    Box() = default;
    explicit Box(
        int t   , int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ): Above_lc_hv_WHD(
        t, l, c,
        h, v,
        W, H, D
    ), Geo() {}

protected:
    void aboveDidChange(Above_p p) override {
        if (p) _level = p->level() + 1;
        return;
    }
    void leftDidChange(Below_p left_p) override {
        if (left_p) _level = left_p->level();
        return;
    }
    public:
    int level() const override final { return _level; }
    bool isBox() const override { return true; }
    PairBelow_p make_bdries() override {
        auto b_p = get_below_p();
        auto r_p = get_rbelow();
        if (b_p && r_p) {
            tlc_s tlc;
            auto next_p = b_p;
            do {
                if (!next_p->isRef()) {
                    tlc = tlc_s(next_p->t(), next_p->l(), next_p->c());
                    break;
                }
            } while((next_p = next_p->get_right_p()));
            auto first_p = BoxBdry::make_p(
                tlc.t, tlc.l, tlc.c,
                w_V(), n_V()
            );
            next_p = r_p;
            do {
                if (!next_p->isRef()) {
                    tlc = tlc_s(next_p->t(), next_p->l(), next_p->c());
                    break;
                }
            } while((next_p = next_p->get_left_p()));
            auto second_p = BoxBdry::make_p(
                tlc.t, tlc.l, tlc.c,
                w_V(), s_V()
            );
            set_below_p(first_p);
            r_p->set_right_p(second_p);
            return {first_p, second_p};
        }
        return {nullptr, nullptr};
    }
    Bi_s closest_deep_below_v2(const hv_s &hv) const override;
    Bi_s eq_closest_below_v2(const hv_s &hv) const override;

    lr_Bi_s eq_get_closest_below_in_box(const hv_s &hv) const;
    lr_Bi_s eq_get_closest_below_in_box_v3(const hv_s &hv) const;
    
    lr_Bi_s eq_get_closest_below_in_vbox_v2(const hv_s &hv) const override;
    Bi_s closest_container(const hv_s &hv) override;

    /**
     * @brief The deepest container that contains the hit
     * 
     * @param hit 
     * @return Node_p 
     */
    Bi_s closest_container_below(const hv_s &hit) const override
    {
        // we iterate over all the nodes below
        Bi_s bi;
        if (bi.p) {
            Below_p p = bi.p;
            bi.i = p->distance_to(hit);
            auto end_p = get_rbelow();
            while ((p != end_p) && (p = p->get_next_p())) {
                int d = p->distance_to(hit);
                if (d < bi.i) {
                    bi.i = d;
                    bi.p = p;
                }
            }
        }
        return bi;
    }

    std::ostream & box_log(std::ostream &o, char prefix, char suffix) const {
        o << prefix << _t << ',' << _l;
        if (_c>0) {
            o << ',' << _c;
        }
        o << ':' << _h << ',' << _v;
        o << ':' << _W << ',' << _H << ',' << _D << std::endl;
        if (_below_p) {
            _below_p->log(o) << std::endl;
        }
        o << suffix;
        return o;
    }
};

class VBox final: public Box
{
    public:
    Type type() const final override { return Type::VBox; }
    std::string type_name() const final override { return "VBox"; }
    char prefix() const override { return Prefix::BEGIN_VBOX; }

    VBox(const VBox&) = delete;
    VBox& operator=(const VBox&) = delete;
    VBox(VBox&&) = delete;
    VBox& operator=(VBox&&) = delete;
    VBox() = delete;

    protected:
    explicit VBox(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
   ): Box(
        t, l, c,
        h, v,
        W, H, D
    ) {}
 
    public:   
    static std::shared_ptr<VBox> make_p(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ) {
        return std::shared_ptr<VBox>(
            new VBox(
                t, l, c,
                h, v,
                W, H, D
            )
        );
    };

    bool isVBox() const override { return true; }
    VBox_p asVBox_p() override {
        return std::dynamic_pointer_cast<VBox>(shared_from_this());
    }
    std::ostream & log(std::ostream &o) const override {
        return box_log(o, '[', ']');
    }
};

/**
 *  The hbox model.
 *  It contains V variants of geometrical information.
 *  It happens that hboxes contain material that is not used to compute
 *  the bounding box. Some letters may appear out of the box given by TeX.
 *  In such a situation, the visible bouding box is bigger hence the V variant.
 *  Only hboxes have such variant. It does not make sense for void boxes
 *  and it is not used here for vboxes.
 *  - author: JL
 */
class HBox final: public Box
{
    protected:
    int _mean_l = 0;
    int _weight = 0;

    int _w_V = std::numeric_limits<int>::max();
    int _n_V = std::numeric_limits<int>::max();
    int _e_V = std::numeric_limits<int>::min();
    int _s_V = std::numeric_limits<int>::min();

    public:
    int w_V() const override final { return _w_V; }
    int n_V() const override final { return _n_V; }
    int e_V() const override final { return _e_V; }
    int s_V() const override final { return _s_V; }

    public:
    Type type() const final override { return Type::HBox; }
    std::string type_name() const final override { return "HBox"; }
    char prefix() const override { return Prefix::BEGIN_HBOX; }

    HBox(const HBox&) = delete;
    HBox& operator=(const HBox&) = delete;
    HBox(HBox&&) = delete;
    HBox& operator=(HBox&&) = delete;
    HBox() = delete;

    protected:
    explicit HBox(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ): Box(
        t, l, c,
        h, v,
        W, H, D
    ) {
        auto wnes = this->wnes();
        _w_V = wnes.w;
        _n_V = wnes.n;
        _e_V = wnes.e;
        _s_V = wnes.s;
    }
 
    public:
    static HBox_p make_p(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ) {
        return HBox_p(
            new HBox(
                t, l, c,
                h, v,
                W, H, D
            )
        );
    };
    
    Status fit_to_hv_V(const Below_p &p);

    void fit_to_wnes_V(const Below_p& p) override 
    {
        if (p) {
            auto
            i = p->w_V(); if (_w_V > i) _w_V = i;
            i = p->n_V(); if (_n_V > i) _n_V = i;
            i = p->e_V(); if (_e_V < i) _e_V = i;
            i = p->s_V(); if (_s_V < i) _s_V = i;
        }
    }
    
    wnes_s wnes_V() const override final {
        return wnes_s(_w_V, _n_V, _e_V, _s_V);
    };

    bool isHBox() const override { return true; }
    HBox_p asHBox_p() override {
        return std::dynamic_pointer_cast<HBox>(shared_from_this());
    }
 
    int mean_l() const override { return _mean_l; }
    int set_mean_l(int i) override { return (_mean_l = i); }

    int weight() const override { return _weight; }
    int set_weight(int i) override { return (_weight = i); }

    lr_Bi_s eq_get_closest_below_in_hbox_v2(hv_s hv) const override;

    bool contains(const hv_s &hv) const override final {
        return hv.h >= w() && hv.h <= e() && hv.v >= n() && hv.v <= s();
    }

    bool contains_V(const hv_s &hv) const override final {
        return hv.h >= _w_V && hv.h <= _e_V && hv.v >= _n_V && hv.v <= _s_V;
    }
    std::ostream & log(std::ostream &o) const override {
        return box_log(o, '(', ')');
    }
};  

inline Status HBox::fit_to_hv_V(const Below_p &p)
{
    if (p) {
        auto
        i = p->h(); if (_w_V > i) _w_V = i; if (_e_V < i) _e_V = i;
        i = p->v(); if (_n_V > i) _n_V = i; if (_s_V < i) _s_V = i;
    }
    return Status::Done;
}

using Box_p = std::shared_ptr<Box>;

inline std::ostream& operator<<(std::ostream& os, const Box_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

using VBox_p = std::shared_ptr<VBox>;

inline std::ostream& operator<<(std::ostream& os, const VBox_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

using HBox_p = std::shared_ptr<HBox>;

inline std::ostream& operator<<(std::ostream& os, const HBox_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

}; // namespace SyncTeXpp

#endif // SYNCTEX_BOX_HPP
