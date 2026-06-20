/**
 * SyncTeXNodeProxy.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

/**
 * @brief Proxy to SyncTeX nodes.
 * 
 * Used by a Scanner instance.
 */

#ifndef SYNCTEX_PROXY_HPP
#define SYNCTEX_PROXY_HPP

#include <memory>
#include <string>
#include "SyncTeXBox.hpp"

namespace SyncTeXpp {

class ProxyHBox;
using ProxyHBox_p = std::shared_ptr<ProxyHBox>;

class ProxyVBox;
using ProxyVBox_p = std::shared_ptr<ProxyVBox>;

class Proxy;
using Proxy_p = std::shared_ptr<Proxy>;


class Proxy:
    public With_hv<Below>,
    public Geo_hv
{
    Below_p _target_p;

    public:
    Type type() const override { return Type::Proxy; }
    std::string type_name() const override { return "Proxy"; };
    char prefix() const override { return Prefix::NONE; }

    Proxy(const Proxy&) = delete;
    Proxy& operator=(const Proxy&) = delete;
    Proxy(Proxy&&) = delete;
    Proxy& operator=(Proxy&&) = delete;
    Proxy() = delete;

    explicit Proxy(
        Below_p target_p
    ): _target_p(target_p) {}
 
    public:   
    static Proxy_p make_p(
        Ref_p ref_p,
        Below_p target_p
    );
    int t() const override { return _target_p ? _target_p->t(): 0; }
    int l() const override { return _target_p ? _target_p->l(): 0; }
    int c() const override { return _target_p ? _target_p->c(): 0; }
    int W() const override { return _target_p ? _target_p->W(): 0; }
    int H() const override { return _target_p ? _target_p->H(): 0; }
    int D() const override { return _target_p ? _target_p->D(): 0; }
    
    // ni_s h_relative_to(const hv_s &hit) override;
    // ni_s v_relative_to(const hv_s &hit) override;
    // int distance(const hv_s &hit) override;
};

class ProxyHBox final:
    public Proxy
{
    public:
    Type type() const override { return Type::ProxyHBox; }
    std::string type_name() const override { return "ProxyHBox"; };
    char prefix() const override { return Prefix::NONE; }

    ProxyHBox(const ProxyHBox&) = delete;
    ProxyHBox& operator=(const ProxyHBox&) = delete;
    ProxyHBox(ProxyHBox&&) = delete;
    ProxyHBox& operator=(ProxyHBox&&) = delete;
    ProxyHBox() = delete;

    explicit ProxyHBox(
        Below_p target_p
    ): Proxy(target_p) {}

};

class ProxyVBox final:
    public Proxy
{
    public:
    Type type() const override { return Type::ProxyVBox; }
    std::string type_name() const override { return "ProxyVBox"; };
    char prefix() const override { return Prefix::NONE; }

    ProxyVBox(const ProxyVBox&) = delete;
    ProxyVBox& operator=(const ProxyVBox&) = delete;
    ProxyVBox(ProxyVBox&&) = delete;
    ProxyVBox& operator=(ProxyVBox&&) = delete;
    ProxyVBox() = delete;

    explicit ProxyVBox(
        Below_p target_p
    ): Proxy(target_p) {}

};

inline Proxy_p Proxy::make_p(
    Ref_p ref_p,
    Below_p target_p
) {
    if (ref_p && target_p) {
        Proxy_p p = nullptr;
        if (target_p->isHBox()) {
            p = Proxy_p(new ProxyHBox(target_p));
        } else {
            p = Proxy_p(new ProxyVBox(target_p));
        }
        p->set_h(ref_p->h());
        p->set_v(ref_p->v() - target_p->H());
        return p;
    }
    return nullptr;
};

class ProxyLast:
    public Proxy
{

};

}; // namespace SyncTeXpp

#endif // SYNCTEX_PROXY_HPP

