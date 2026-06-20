/**
 * SyncTeXWHD.cpp
 * 
 * This file is part of the SyncTeX library.
 * 
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include <limits>
#include <cstring>
#include "SyncTeXProxy.hpp"

namespace SyncTeXpp {

// ni_s Proxy::h_relative_to(const hv_s &hit)
// {
//     hv_s otherHit = hit;
//     otherHit.h -= h();
//     otherHit.v -= v();
//     ni_s nd = target()->h_relative_to(otherHit);
//     nd.node_p = shared_from_this();
//     return nd;
// }

#define SYNCTEX_HPP_h_relative_to(CLS)    \
ni_s CLS::h_relative_to(const hv_s &hit)  \
{                                                       \
    hv_s otherHit = hit;                                \
    otherHit.h -= h();                                  \
    otherHit.v -= v();                                  \
    ni_s nd = target()->h_relative_to(otherHit);        \
    nd.node_p = shared_from_this();                     \
    return nd;                                          \
}
SYNCTEX_HPP_h_relative_to(Proxy)
SYNCTEX_HPP_h_relative_to(ProxyLast)

#undef SYNCTEX_HPP_h_relative_to

// ni_s Proxy::v_relative_to(const hv_s &hit) { 
//     auto otherHit = hit;
//     otherHit.h -= h();
//     otherHit.v -= v();
//     auto target_p = target();
//     if (target_p) { return target_p->v_relative_to(otherHit); }
//     return { shared_from_this(), std::numeric_limits<int>::max() };
// }

#define SYNCTEX_HPP_v_relative_to(CLS)    \
ni_s CLS::v_relative_to(const hv_s &hit)  \
{                                                       \
    auto otherHit = hit;                                \
    otherHit.h -= h();                                  \
    otherHit.v -= v();                                  \
    auto target_p = target();                           \
    if (target_p) {                                     \
        return target_p->v_relative_to(otherHit);       \
    }                                                   \
    return { shared_from_this(), std::numeric_limits<int>::max() };  \
}

SYNCTEX_HPP_v_relative_to(Proxy)
SYNCTEX_HPP_v_relative_to(ProxyLast)

#define SYNCTEX_CPP_distance(CLS)               \
int CLS::distance(const hv_s &hit)              \
{                                               \
    hv_s otherHit = hit;                        \
    otherHit.h -= h();                          \
    otherHit.v -= v();                          \
    auto target_p = target();                   \
    if (target_p) {                             \
        return target_p->distance(otherHit);    \
    }                                           \
    return std::numeric_limits<int>::max();     \
}

SYNCTEX_CPP_distance(Proxy)
SYNCTEX_CPP_distance(ProxyLast)

#undef SYNCTEX_CPP_distance

} // namespace SyncTeXpp
