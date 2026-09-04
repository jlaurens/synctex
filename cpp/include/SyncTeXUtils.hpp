/**
 * SyncTeXUtils.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

/**
 * @brief Utilities.
 *
 */

#ifndef SYNCTEX_UTILS_HPP
#define SYNCTEX_UTILS_HPP

#include <string>
#include <string_view>
#include <memory>
#include <utility>

#if defined(SYNCTEX_TEST)
#include <ostream>
#endif

#ifndef SYNCTEX_TEST_VISITOR
#define SYNCTEX_TEST_VISITOR
#endif

#ifndef SYNCTEX_COVERAGE
#define SYNCTEX_COVERAGE(WHAT) true
#define SYNCTEX_R_COVERAGE(WHAT) true
#define SYNCTEX_COVERAGE_PRINT(MSG) true
#endif

namespace SyncTeXpp {

class Node;
using Node_p = std::shared_ptr<Node>;
using Node_v = std::vector<Node_p>;
/**
 * @brief Type for status as return value
 *
 */
enum class Status: int
{
    Done,                   //  0: When the function returns the value it was asked for: It must be the biggest one
    EndOfCapacity,          //  1: When the maximum capacity of the buffer has been reached:
    EndOfData,              //  2: When the end of the synctex file has been reached:
    Failed,                 //  3: When the function could not return the value it was asked for:
    Error,                  //  4: Generic error
    ErrorFS,                //  5: Generic FS error
    ErrorSeek,              //  6: Seek file failure
    ErrorMemory,            //  7: Seek file failure
    ErrorDataUnexpected,    //  8: The synctex file is corrupted
    ErrorDataMissing,       //  9: The synctex file is corrupted
    ErrorDataFloat,         // 10: The synctex file is corrupted
    ErrorDataUnit,          // 11: The synctex file is corrupted
    ErrorDataPreamble,      // 12: Problem reading the Preamble records
    ErrorDataContent,       // 13: Problem reading the Content records
    ErrorDataPostamble,     // 14: Problem reading the Postamble
    ErrorDataNoAnchor,      // 15: Missing anchor
    ErrorDataBadRefTag,     // 16: No form for a ref tag
    ErrorBadArgument,       // 17: Parameter error
    ErrorLogical,           // 18: Logical error, never reached in theory
};


struct tlc_s {
    int t = 0;
    int l = 0;
    int c = 0;
    tlc_s(
        int t = 0, int l = 0, int c = 0
    ): t(t), l(l), c(c) {}
    bool operator==(const tlc_s& o) const {
        return t == o.t && l == o.l && c == o.c;
    }
    bool operator!=(const tlc_s& o) const {
        return ! operator==(o);
    }
};

struct hv_s {
    int h = 0;
    int v = 0;
    hv_s(
        int h = 0, int v = 0
    ): h(h), v(v) {}
    bool operator==(const hv_s& o) const {
        return h == o.h && v == o.v;
    };
    bool operator!=(const hv_s& o) const {
        return ! operator==(o);
    }
};


struct we_s {
    int w = 0;
    int e = 0;
    we_s(
        int w = 0,
        int e = 0
    ): w(w), e(e) {
        if (w > e) std::swap(this->w, this->e);
    }
    bool operator==(const we_s& o) const {
        return w == o.w && e == o.e;
    };
    bool operator!=(const we_s& o) const {
        return ! operator==(o);
    }
};

struct ns_s {
    int n = 0;
    int s = 0;
    ns_s(
        int n = 0,
        int s = 0
    ): n(n), s(s) {
        if (n > s) std::swap(this->n, this->s);
    }
    bool operator==(const ns_s& o) const {
        return n == o.n && s == o.s;
    };
    bool operator!=(const ns_s& o) const {
        return ! operator==(o);
    }
};

struct wnes_s {
    int w = 0;
    int n = 0;
    int e = 0;
    int s = 0;
    wnes_s(
        int w = 0, int n = 0,
        int e = 0, int s = 0
    ): w(w), n(n), e(e), s(s) {
        if (w > e) std::swap(this->w, this->e);
        if (n > s) std::swap(this->n, this->s);
    }
    bool operator==(const wnes_s& o) const {
        return w == o.w && n == o.n && e == o.e && s == o.s;
    };
    bool operator!=(const wnes_s& o) const {
        return ! operator==(o);
    }
    hv_s wn() const { return {w, n}; }
    hv_s es() const { return {e, s}; }
    we_s we() const { return {w, e}; }
    ns_s ns() const { return {n, s}; }
};

struct tlc_hv_s: public tlc_s, public hv_s {
    tlc_hv_s(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0
    ): tlc_s(t, l, c), hv_s(h, v) {}
    bool operator==(const tlc_hv_s& o) const {
        return tlc_s::operator==(o) && hv_s::operator==(o);
    }
    bool operator!=(const tlc_hv_s& o) const {
        return ! operator==(o);
    }
};

struct t_hv_s: public hv_s {
    int t = 0;
    t_hv_s(
        int t = 0,
        int h = 0, int v = 0
    ): hv_s(h, v), t(t) {}
    bool operator==(const tlc_hv_s& o) const {
        return t==o.t && hv_s::operator==(o);
    }
    bool operator!=(const tlc_hv_s& o) const {
        return ! operator==(o);
    }
};

struct tlc_hv_W_s: public tlc_hv_s {
    int W = 0;
    tlc_hv_W_s(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0
    ): tlc_hv_s(t, l, c, h, v), W(W) {}
    bool operator==(const tlc_hv_W_s& o) const {
        return tlc_hv_s::operator==(o) && W == o.W;
    }
    bool operator!=(const tlc_hv_W_s& o) const {
        return ! operator==(o);
    }
};

struct tlc_hv_WHD_s: public tlc_hv_W_s {
    int H = 0;
    int D = 0;
    tlc_hv_WHD_s(
        int t = 0, int l = 0, int c = 0,
        int h = 0, int v = 0,
        int W = 0, int H = 0, int D = 0
    ): tlc_hv_W_s(t, l, c, h, v, W), H(H), D(D) {}
    bool operator==(const tlc_hv_WHD_s& o) const {
        return tlc_hv_W_s::operator==(o) && H == o.H && D == o.D;
    }
    bool operator!=(const tlc_hv_WHD_s& o) const {
        return ! operator==(o);
    }
};

struct nd_s {
    Node_p node_p;
    int distance;
};

struct lr_nd_s {
    nd_s left;
    nd_s right;
};

/**
 * @brief size+status structure
 *
 * Used to return multiple values from a function.
 */
struct zr_s {
    /** a size */
    size_t size;
    /** a status error */
    Status status;
};

/**
 * @brief node+status structure
 *
 * Used to return multiple values from a function.
 */
struct nr_s {
    /** node component */
    Node_p node_p;
    /** error status component */
    Status status;
};

struct nnr_s {
    Node_p first_p;
    Node_p last_p;
    Status status;
};

#ifdef SYNCTEX_TEST
inline std::ostream& operator<<(std::ostream& os, const wnes_s& p) {
    return os << "wnes_s(" << p.w << "," << p.n 
                << "," << p.e << "," << p.s << ")";
}
inline std::ostream& operator<<(std::ostream& os, const tlc_s& p) {
    return os   << "tlc_s(" << p.t << "," << p.l << "," << p.c << ")";
}
inline std::ostream& operator<<(std::ostream& os, const t_hv_s& p) {
    return os   << "t_hv_s(" << p.t << ","
                << ":" << p.h << "," << p.v << ")";
}
inline std::ostream& operator<<(std::ostream& os, const tlc_hv_s& p) {
    return os   << "tlc_hv_s(" << p.t << "," << p.l << "," << p.c
                << ":" << p.h << "," << p.v << ")";
}
inline std::ostream& operator<<(std::ostream& os, const tlc_hv_WHD_s& p) {
    return os   << "tlc_hv_WHD_s(" << p.t << "," << p.l << "," << p.c
                << ":" << p.h << "," << p.v
                << ":" << p.W << "," << p.H << "," << p.D << ")";
}

#endif

};

#if defined(SYNCTEX_TEST)
#include "TestTools.hpp"
#endif

#endif // SYNCTEX_UTILS_HPP
