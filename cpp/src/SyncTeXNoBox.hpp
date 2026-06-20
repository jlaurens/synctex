/**
 * SyncTeXNoBox.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

// #pragma once

/**
 * @brief Nodes with no children except void boxes and alike.
 *
 * Class hierarchy:
 * 
 * Geo
 * ├─Geo_hv
 * │ │  Below_lc_hv
 * │ └──┴─┬─ Math
 * │      ├─ Glue
 * │      ├─ Bdry
 * │      └─ BoxBdry
 * │    Below_lc_hv
 * ├────┴─── Kern
 * │    Below_lc_hv_WHD
 * └────┴─── Void
 *           ├─ Rule
 *           ├─ VoidHBox
 *           └─ VoidVBox
 * 
 */

/**
 * @brief Nodes to represent SyncTeX data.
 * 
 * Used by a Scanner instance.
 */

#ifndef SYNCTEX_NO_BOX_HPP
#define SYNCTEX_NO_BOX_HPP

#include "SyncTeXPrefix.hpp"
#include "SyncTeX_tlc_hv_WHD.hpp"
#include "SyncTeXGeo.hpp"

namespace SyncTeXpp {

class Geo_hv: public Geo {
    public:
    int w() const override { return h(); }
    int n() const override { return v(); }
    int e() const override { return h(); }
    int s() const override { return v(); }

    int h_distance_to(const hv_s &hv) const override {
        return std::abs(h() - hv.h);
    }
    int h_relative_to(const hv_s &hv) const override {
        return h() - hv.h; // < 0 <=> the left of hv
    }
    int v_distance_to(const hv_s &hv) const override {
        return std::abs(v() - hv.v);
    }
    int v_relative_to(const hv_s &hv) const override {
        return v() - hv.v; // < 0 <=> above hv
    }
};

class Math;
using Math_p = std::shared_ptr<Math>;

class Math final:
    public Below_lc_hv,
    public Geo_hv
{
    public:
    Type type() const override { return Type::Math; }
    std::string type_name() const override { return "Math"; };
    char prefix() const override { return Prefix::MATH; }

    Math(const Math&) = delete;
    Math& operator=(const Math&) = delete;
    Math(Math&&) = delete;
    Math& operator=(Math&&) = delete;
    Math() = delete;

    protected:
    explicit Math(
        int t = 0,
        int l = 0,
        int c = 0,
        int h = 0,
        int v = 0
    ): Below_lc_hv(
        t, l, c,
        h, v
    ), Geo_hv() {}

    public:
    static std::shared_ptr<Math> make_p(
        int t = 0,
        int l = 0,
        int c = 0,
        int h = 0,
        int v = 0
    ) {
        return std::shared_ptr<Math>(
            new Math(
                t, l, c,
                h, v
            )
        );
    };
};

inline std::ostream& operator<<(std::ostream& os, const Math_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

class Glue;
using Glue_p = std::shared_ptr<Glue>;

class Glue final:
    public Below_lc_hv,
    public Geo_hv
{
    public:
    Type type() const override { return Type::Glue; }
    std::string type_name() const override { return "Glue"; };
    char prefix() const override { return Prefix::GLUE; }

    Glue(const Glue&) = delete;
    Glue& operator=(const Glue&) = delete;
    Glue(Glue&&) = delete;
    Glue& operator=(Glue&&) = delete;
    Glue() = delete;

    protected:
    explicit Glue(
        int t = 0,
        int l = 0,
        int c = 0,
        int h = 0,
        int v = 0
    ): Below_lc_hv(
        t, l, c,
        h, v
    ), Geo_hv() {}

    public:
    static std::shared_ptr<Glue> make_p(
        int t = 0,
        int l = 0,
        int c = 0,
        int h = 0,
        int v = 0
    ) {
        return std::shared_ptr<Glue>(
            new Glue(
                t, l, c,
                h, v
            )
        );
    };
};

inline std::ostream& operator<<(std::ostream& os, const Glue_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

class Bdry;

using Bdry_p = std::shared_ptr<Bdry>;

class Bdry final:
    public Below_lc_hv,
    public Geo_hv
{
    public:
    Type type() const override { return Type::Bdry; }
    std::string type_name() const override { return "Bdry"; };
    char prefix() const override { return Prefix::BOUNDARY; }

    Bdry(const Bdry&) = delete;
    Bdry& operator=(const Bdry&) = delete;
    Bdry(Bdry&&) = delete;
    Bdry& operator=(Bdry&&) = delete;
    Bdry() = delete;

    protected:
    explicit Bdry(
        int t = 0,
        int l = 0,
        int c = 0,
        int h = 0,
        int v = 0
    ): Below_lc_hv(
        t, l, c,
        h, v
    ), Geo_hv() {}

    public:
    static std::shared_ptr<Bdry> make_p(
        int t = 0,
        int l = 0,
        int c = 0,
        int h = 0,
        int v = 0
    ) {
        return std::shared_ptr<Bdry>(
            new Bdry(
                t, l, c,
                h, v
            )
        );
    };

    std::ostream & log(std::ostream &os) const override {
        return os << '|' << t() << ":" << h() << "," << v();
    };

};

inline std::ostream& operator<<(std::ostream& os, const Bdry_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

class BoxBdry;
using BoxBdry_p = std::shared_ptr<BoxBdry>;

class BoxBdry final:
    public Below_lc_hv,
    public Geo_hv
{
    public:
    Type type() const override { return Type::BoxBdry; }
    std::string type_name() const override { return "BoxBdry"; };
    char prefix() const override { return '|'; }

    BoxBdry(const BoxBdry&) = delete;
    BoxBdry& operator=(const BoxBdry&) = delete;
    BoxBdry(BoxBdry&&) = delete;
    BoxBdry& operator=(BoxBdry&&) = delete;
    BoxBdry() = delete;

    protected:
    explicit BoxBdry(
        int t = 0,
        int l = 0,
        int c = 0,
        int h = 0,
        int v = 0
    ): Below_lc_hv(
        t, l, c,
        h, v
    ), Geo_hv() {}

    public:
    static std::shared_ptr<BoxBdry> make_p(
        int t = 0,
        int l = 0,
        int c = 0,
        int h = 0,
        int v = 0
    ) {
        return std::shared_ptr<BoxBdry>(
            new BoxBdry(
                t, l, c,
                h, v
            )
        );
    };
};

inline std::ostream& operator<<(std::ostream& os, const BoxBdry_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

class Ref;
using Ref_p = std::shared_ptr<Ref>;
using Ref_v = std::vector<Ref_p>;

class Ref: public With_hv<Below>, public Geo_hv
{
    Ref_p _mate;
    public:
    Type type() const override { return Type::Ref; }
    std::string type_name() const override { return "Ref"; };
    char prefix() const override { return 'f'; }

    Ref(const Ref&) = delete;
    Ref& operator=(const Ref&) = delete;
    Ref(Ref&&) = delete;
    Ref& operator=(Ref&&) = delete;

    protected:
    Ref() = default;
    explicit Ref(
        int t,
        int h = 0, int v = 0
    ): With_hv<Below>(t, h, v), Geo_hv() {}

    public:
    static Ref_p make_p(
        int t = 0,
        int h = 0, int v = 0
    ) {
        return Ref_p( new Ref( t, h, v ) );
    };

    Ref_p mate () const { return _mate; }
    Ref_p set_mate (Ref_p p) { return _mate = p; }
    
    bool isRef() const override { return true; }

    // ni_s h_relative_to(const hv_s &hit) override;
    // ni_s v_relative_to(const hv_s &hit) override;
    std::ostream & log(std::ostream &o) const override {
        return o << prefix() << t() << ':' << h() << ',' << v();
    }

};

inline std::ostream& operator<<(std::ostream& os, const Ref_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

class Kern;
using Kern_p = std::shared_ptr<Kern>;

class Kern final: public Below_lc_hv_W, Geo
{
    public:
    Type type() const override { return Type::Kern; }
    std::string type_name() const override { return "Kern"; };
    char prefix() const override { return 'k'; }

    Kern(const Kern&) = delete;
    Kern& operator=(const Kern&) = delete;
    Kern(Kern&&) = delete;
    Kern& operator=(Kern&&) = delete;

    protected:
    Kern() = default;
    explicit Kern(
        int t    , int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0
    ): Below_lc_hv_W(
        t, l, c,
        h, v,
        -W // BEWARE: The hv is recorded by tex *after* the cursor change
    ), Geo() {}

    public:
    static Kern_p make_p(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0
    ) {
        return Kern_p( new Kern(
            t, l, c,
            h, v,
            W
        ) );
    };

    int v_distance_to(const hv_s &hv) const override {
        return std::abs(v() - hv.v);
    }
    int v_relative_to(const hv_s &hv) const override {
        return v() - hv.v; // < 0 <=> above hv
    }
};

inline std::ostream& operator<<(std::ostream& os, const Kern_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

class Void:
    public Below_lc_hv_WHD,
    public Geo
{
    public:
    Void(const Void&) = delete;
    Void& operator=(const Void&) = delete;
    Void(Void&&) = delete;
    Void& operator=(Void&&) = delete;
    Void() = delete;

    protected:
    explicit Void(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ): Below_lc_hv_WHD(
        t, l, c,
        h, v,
        W, H, D
    ), Geo() {}
    int distance_to(const hv_s &hv) const override
    {
        return Geo::distance_to(wnes(), hv);
    }
};

class Rule: public Void
{
    public:
    Type type() const override { return Type::Rule; }
    std::string type_name() const override { return "Rule"; }
    char prefix() const override { return 'r' ; }

    Rule(const Rule&) = delete;
    Rule& operator=(const Rule&) = delete;
    Rule(Rule&&) = delete;
    Rule& operator=(Rule&&) = delete;
    Rule() = delete;

    protected:
    explicit Rule(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ): Void(
        t, l, c,
        h, v,
        W, H, D
    ) {}
 
    public:   
    static std::shared_ptr<Rule> make_p(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ) {
        return std::shared_ptr<Rule>( new Rule(
            t, l, c,
            h, v,
            W, H, D
        ) );
    };
};

using Rule_p = std::shared_ptr<Rule>;

inline std::ostream& operator<<(std::ostream& os, const Rule_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

class VoidVBox;
using VoidVBox_p = std::shared_ptr<VoidVBox>;

class VoidVBox: public Void
{
    public:
    Type type() const final override { return Type::VoidVBox; }
    std::string type_name() const final override { return "VoidVBox"; }
    char prefix() const override final { return 'v'; }

    VoidVBox(const VoidVBox&) = delete;
    VoidVBox& operator=(const VoidVBox&) = delete;
    VoidVBox(VoidVBox&&) = delete;
    VoidVBox& operator=(VoidVBox&&) = delete;
    VoidVBox() = delete;

    protected:
    explicit VoidVBox(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ): Void( 
        t, l, c,
        h, v,
        W, H, D
    ) {}
 
    public:
    static std::shared_ptr<VoidVBox> make_p(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ) {
        return std::shared_ptr<VoidVBox>( new VoidVBox(
            t, l, c,
            h, v,
            W, H, D
        ) );
    };
};

inline std::ostream& operator<<(std::ostream& os, const VoidVBox_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

class VoidHBox;
using VoidHBox_p = std::shared_ptr<VoidHBox>;

class VoidHBox: public Void
{
    public:
    Type type() const final override { return Type::VoidHBox; }
    std::string type_name() const final override { return "VoidHBox"; }
    char prefix() const override final { return 'h'; }

    VoidHBox(const VoidHBox&) = delete;
    VoidHBox& operator=(const VoidHBox&) = delete;
    VoidHBox(VoidHBox&&) = delete;
    VoidHBox& operator=(VoidHBox&&) = delete;
    VoidHBox() = delete;

    protected:
    explicit VoidHBox(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ): Void( 
        t, l, c,
        h, v,
        W, H, D
    ) {}
 
    public:
    static std::shared_ptr<VoidHBox> make_p(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ) {
        return std::shared_ptr<VoidHBox>( new VoidHBox(
            t, l, c,
            h, v,
            W, H, D
        ) );
    };
    int distance_to(const hv_s &hv) const override
    {
        return Geo::distance_to(wnes(), hv);
    }
};

inline std::ostream& operator<<(std::ostream& os, const VoidHBox_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

}; // namespace SyncTeXpp

#endif // SYNCTEX_NO_BOX_HPP

