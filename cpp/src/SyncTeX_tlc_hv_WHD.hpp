/**
 * SyncTeX_tlc_hv_WHD.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

/**
 * @brief Attributes
 * 
 * Class hierarchy:
 * 
 * With_hv<...>
 * └─ With_lc_hv<...>
 *    │ Below
 *    ├─┴─ Below_lc_hv
 *    │    └─ Below_lc_hv_W
 *    │       └─ Below_lc_hv_WHD
 *    │ Above
 *    └─┴─ Above_lc_hv_WHD
 * 
 */

#ifndef SYNCTEX_TLC_HV_WHD_HPP
#define SYNCTEX_TLC_HV_WHD_HPP

#include <iostream>

#include "SyncTeXTree.hpp"

namespace SyncTeXpp {

template <class T>
class With_hv: public T
{
    protected:
    int _h = 0;
    int _v = 0;

    public:
    With_hv(const With_hv&) = delete;
    With_hv& operator=(const With_hv&) = delete;
    With_hv(With_hv&&) = delete;
    With_hv& operator=(With_hv&&) = delete;
    virtual ~With_hv() = default;

    protected:
    With_hv() = default;
    explicit With_hv(
        int t,
        int h = 0, int v = 0
    ):
        T(t), _h(h), _v(v) {}
    public:
    int h() const final override { return _h; }
    int v() const final override { return _v; }

    int set_h(int i) { return _h = i; }
    int set_v(int i) { return _v = i; }

    /**
     * @brief Set the hv combo from the argument.
     * 
     * @return the changed hv combo, if any...
     */
    hv_s same_hv(Node_p p) {
        if (p) {
            set_h(p->h());
            set_v(p->v());
        }
        return T::hv();
    }
};

template<class T>
class With_lc_hv: public With_hv<T>
{
    protected:
    int _l = 0;
    int _c = 0;

    public:
    With_lc_hv(const With_lc_hv&) = delete;
    With_lc_hv& operator=(const With_lc_hv&) = delete;
    With_lc_hv(With_lc_hv&&) = delete;
    With_lc_hv& operator=(With_lc_hv&&) = delete;
    virtual ~With_lc_hv() = default;

    protected:
    With_lc_hv() = default;
    explicit With_lc_hv(
        int t, int l = 0, int c = 0,
        int h = 0, int v = 0
    ):
        With_hv<T>(t, h, v), _l(l), _c(c) {}
    public:
    int l() const final override { return _l; }
    int c() const final override { return _c; }

    int set_l(int i) { return _l = i; }
    int set_c(int i) { return _c = i; }
};

class Below_lc_hv: public With_lc_hv<Below>
{
    public:
    Below_lc_hv(const Below_lc_hv&) = delete;
    Below_lc_hv& operator=(const Below_lc_hv&) = delete;
    Below_lc_hv(Below_lc_hv&&) = delete;
    Below_lc_hv& operator=(Below_lc_hv&&) = delete;
    virtual ~Below_lc_hv() = default;

    protected:
    Below_lc_hv() = default;
    explicit Below_lc_hv(
        int t, int l = 0, int c = 0,
        int h = 0, int v = 0
    ): With_lc_hv<Below>(t, l, c, h, v) {}

    std::ostream & log(std::ostream &os) const override {
        return os << prefix() << t() << "," << l() << "," << c()
            << ":" << h() << "," << v();
    };
};

class Below_lc_hv_W: public Below_lc_hv
{
    public:
    Below_lc_hv_W(const Below_lc_hv_W&) = delete;
    Below_lc_hv_W& operator=(const Below_lc_hv_W&) = delete;
    Below_lc_hv_W(Below_lc_hv_W&&) = delete;
    Below_lc_hv_W& operator=(Below_lc_hv_W&&) = delete;
    virtual ~Below_lc_hv_W() = default;

    protected:
    int _W = 0;
    Below_lc_hv_W() = default;
    explicit Below_lc_hv_W(
        int t    , int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0
    ):
        Below_lc_hv(
            t, l, c,
            h, v
        ),
        _W(W)  {}

    public:
    int W() const final override { return _W; }
    int set_W(int i) {
        std::cout << "#### SET i to " << i << std::endl;
        return _W = i;
    }
    std::ostream & log(std::ostream &os) const override {
        return Below_lc_hv::log(os) << ":" << W();
    };

    int w() const override {
        if (_W >= 0) {
            return h();
        } else /* if (_W < 0) */ {
            auto hh = h();
            if (hh <= std::numeric_limits<int>::min() - _W) {
                return std::numeric_limits<int>::min();
            }
            return hh + _W;
        }
    }
    int e() const override {
        if (_W <= 0) {
            return h();
        } else /* if (_W > 0) */ {
            auto hh = h();
            if (hh >= std::numeric_limits<int>::max() - _W) {
                return std::numeric_limits<int>::max();
            }
            return hh + _W;
        }
    }
};

class Below_lc_hv_WHD: public Below_lc_hv_W
{
    protected:
    int _H = 0;
    int _D = 0;

    public:
    Below_lc_hv_WHD(const Below_lc_hv_WHD&) = delete;
    Below_lc_hv_WHD& operator=(const Below_lc_hv_WHD&) = delete;
    Below_lc_hv_WHD(Below_lc_hv_WHD&&) = delete;
    Below_lc_hv_WHD& operator=(Below_lc_hv_WHD&&) = delete;
    virtual ~Below_lc_hv_WHD() = default;

    protected:
    Below_lc_hv_WHD() = default;
    explicit Below_lc_hv_WHD(
        int t    , int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ):
        Below_lc_hv_W(
            t, l, c,
            h, v,
            W
        ),
        _H(H), _D(D) {
            if (_W >= 0) {

            }
        }

    public:
    int H() const final override { return _H; }
    int D() const final override { return _D; }

    int set_H(int i) { return _H = i; }
    int set_D(int i) { return _D = i; }

    std::ostream & log(std::ostream &os) const override {
        return Below_lc_hv_W::log(os) << "," << H() << "," << D();
    };

    int n() const override final { return std::min(v()-_H, v()+_D); }
    int s() const override final { return std::max(v()-_H, v()+_D); }
};

class Above_lc_hv_WHD: public With_lc_hv<Above>
{
    protected:
    int _W = 0;
    int _H = 0;
    int _D = 0;

    public:
    Above_lc_hv_WHD(const Above_lc_hv_WHD&) = delete;
    Above_lc_hv_WHD& operator=(const Above_lc_hv_WHD&) = delete;
    Above_lc_hv_WHD(Above_lc_hv_WHD&&) = delete;
    Above_lc_hv_WHD& operator=(Above_lc_hv_WHD&&) = delete;
    virtual ~Above_lc_hv_WHD() = default;

    protected:
    Above_lc_hv_WHD() = default;
    explicit Above_lc_hv_WHD(
        int t    , int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ):
        With_lc_hv<Above>(t, l, c, h, v),
        _W(W), _H(H), _D(D) {}

    public:
    int W() const final override { return _W; }
    int H() const final override { return _H; }
    int D() const final override { return _D; }

    std::ostream & log(std::ostream &os) const override {
        return os << prefix() << t() << "," << l() << "," << c()
            << ":" << h() << "," << v()
            << ":" << W() << "," << H() << "," << D();
    };
    int w() const override final { return std::min(h(), h()+_W); }
    int n() const override final { return std::min(v()-_H, v()+_D); }
    int e() const override final { return std::max(h(), h()+_W); }
    int s() const override final { return std::max(v()-_H, v()+_D); }
    
};
    
};

#endif // SYNCTEX_TLC_HV_WHD_HPP