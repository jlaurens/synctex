/**
 * SyncTeXWHD.cpp
 * 
 * This file is part of the SyncTeX library.
 * It implements the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include <limits>
#include <cstring>
#include "SyncTeXBox.hpp"
#include "SyncTeXProxy.hpp"

namespace SyncTeXpp {

#define SYNCTEX_CPP_distance(CLS)       \
int CLS::distance(const hv_s &hit)      \
{                                       \
    TL_BR_s box = {{h(), v()}, {0, 0}};   \
    box.br = box.tl;                    \
    box.tl.v -= std::abs(g());     \
    box.br.h += std::abs(w());      \
    box.br.v += std::abs(d());     \
    return distance_to_box(hit, box);   \
}
SYNCTEX_CPP_distance(VBox)
SYNCTEX_CPP_distance(ProxyVBox)
SYNCTEX_CPP_distance(HBox)
SYNCTEX_CPP_distance(ProxyHBox)
#undef SYNCTEX_CPP_distance

#define SYNCTEX_CPP_distance(CLS)                   \
int CLS::distance(const hv_s &hit)                  \
{                                                   \
    /*  best of distances from the left edge and right edge*/ \
    TL_BR_s box = {{h(), v()}, {0, 0}};               \
    box.br = box.tl;                                \
    box.br.v += std::abs(d());                  \
    box.tl.v -= std::abs(g());                 \
    int d = distance_to_box(hit, box);                  \
    box.tl.h += std::abs(w());                  \
    box.br.h = box.tl.h;                            \
    return std::min(d, distance_to_box(hit, box));  \
}
SYNCTEX_CPP_distance(VoidHBox)
SYNCTEX_CPP_distance(VoidVBox)

/*  The distance between a point and a box is special.
*  It is not the euclidian distance, nor something similar.
*  We have to take into account the particular layout,
*  and the box hierarchy.
*  Given a box, there are 9 regions delimited by the lines of the edges of the box.
*  The origin being at the top left corner of the page,
*  we also give names to the vertices of the box.
*
*   1 | 2 | 3
*  ---A---B--->
*   4 | 5 | 6
*  ---C---D--->
*   7 | 8 | 9
*     v   v
*/
#define SYNCTEX_HPP_h_relative_to(CLS)    \
int CLS::h_relative_to(const hv_s &hit)  \
{                                                       \
    int d = std::numeric_limits<int>::max();            \
    int min, max, width;                                \
    /*  getting the box bounds, taking into account negative width, height and depth. */ \
    width = this->w();                              \
    min = h();                                          \
    max = min + (width > 0 ? width : -width);           \
    /*  We always have min <= max */                    \
    if (hit.h < min) {                                  \
        /*  regions 1+4+7, result is > 0 */             \
        d = min - hit.h;                                \
    } else if (hit.h > max) {                           \
        /*  regions 3+6+9, result is < 0 */             \
        d = max - hit.h;                                \
    } else {                                            \
        /*  regions 2+5+8, inside the box, except for vertical coordinates */ \
        d = 0;                                          \
    }                                                   \
    return d;                     \
}
SYNCTEX_HPP_h_relative_to(VBox)
SYNCTEX_HPP_h_relative_to(HBox)
SYNCTEX_HPP_h_relative_to(VoidVBox)
SYNCTEX_HPP_h_relative_to(VoidHBox)
SYNCTEX_HPP_h_relative_to(ProxyVBox)
SYNCTEX_HPP_h_relative_to(ProxyHBox)

#undef SYNCTEX_HPP_h_relative_to

/** Rougly speaking, this is:
 *  node's v coordinate - hit point's v coordinate.
 *  If node is at the top of the hit point, then this distance is positive,
 *  if node is at the bottom of the hit point, this distance is negative.
 */
/*  The distance between a point and a box is special.
*  It is not the euclidian distance, nor something similar.
*  We have to take into account the particular layout,
*  and the box hierarchy.
*  Given a box, there are 9 regions delimited by the lines of the edges of the box.
*  The origin being at the top left corner of the page,
*  we also give names to the vertices of the box.
*
*   1 | 2 | 3
*  ---A---B--->
*   4 | 5 | 6
*  ---C---D--->
*   7 | 8 | 9
*     v   v
*/

#define SYNCTEX_HPP_v_relative_to(CLS)    \
ni_s CLS::v_relative_to(const hv_s &hit)  \
{                                                       \
    int d = std::numeric_limits<int>::max();            \
    int min = v();                                      \
    int max = min + std::abs(d());                  \
    min -= std::abs(g());                          \
    /*  We always have min <= max */                    \
    if (hit.v < min) {                                  \
        /*  regions 1+2+3, result is > 0 */             \
        d = min - hit.v;                                \
    } else if (hit.v > max) {                           \
        /*  regions 7+8+9, result is < 0 */             \
        d = max - hit.v;                                \
    } else {                                            \
        /*  regions 4+5+6, inside the box, except for horizontal coordinates */\
        d = 0;                                          \
    }                                                   \
    return { shared_from_this(), d };                   \
}
SYNCTEX_HPP_v_relative_to(VBox)
SYNCTEX_HPP_v_relative_to(VoidVBox)
SYNCTEX_HPP_v_relative_to(ProxyVBox)
SYNCTEX_HPP_v_relative_to(HBox)
SYNCTEX_HPP_v_relative_to(VoidHBox)
SYNCTEX_HPP_v_relative_to(ProxyHBox)
SYNCTEX_HPP_v_relative_to(Rule)

ni_s Ref::v_relative_to(const hv_s &hit)
{
    auto below_p = below_p();
    if (below_p) {
        return below_p->v_relative_to(hit);
    }
    return { shared_from_this(), std::numeric_limits<int>::max() };
}

// was _synctex_eq_get_closest_children_in_box_v2
lr_nd_s Below::eq_get_closest_below_in_box_v2(const hv_s &hit)
{
    if (below_p()) { /* node != nullptr */
        if (isHbox() || isProxyHBox) {
            return eq_get_closest_below_in_hbox_v2(hit);
        } else {
            return eq_get_closest_below_in_vbox_v2(hit);
        }
    }
    return {SYNCTEX_ND_0, SYNCTEX_ND_0};
}

#if 0

// --- (c) 2026 jerome DOT laurens AT ube Traversal ---


// --- Parent Sheet/Form ---


// --- Utility ---

const char* (c) 2026 jerome DOT laurens AT ube::isa() const {
    switch (type_) {
        case NodeType::TYPE_INPUT: return "Input";
        case NodeType::TYPE_SHEET: return "Sheet";
        case NodeType::TYPE_FORM: return "Form";
        case NodeType::TYPE_HBOX: return "HBox";
        case NodeType::TYPE_VBOX: return "VBox";
        case NodeType::TYPE_VOID_VBOX: return "VoidVBox";
        case NodeType::TYPE_HVOID_VBOX: return "HVoidVBox";
        case NodeType::TYPE_RULE: return "Rule";
        case NodeType::TYPE_GLUE: return "Glue";
        case NodeType::TYPE_KERN: return "Kern";
        case NodeType::TYPE_MATH: return "Math";
        case NodeType::TYPE_BOUNDARY: return "Boundary";
        case NodeType::TYPE_REF: return "Ref";
        case NodeType::TYPE_PAR: return "Par";
        case NodeType::TYPE_LINE: return "Line";
        case NodeType::TYPE_CHAR: return "Char";
        case NodeType::TYPE_ANCHOR: return "Anchor";
        case NodeType::TYPE_XREF: return "XRef";
        case NodeType::TYPE_KERN_NODE: return "KernNode";
        case NodeType::TYPE_GLUE_NODE: return "GlueNode";
        case NodeType::TYPE_LIGATURE: return "Ligature";
        case NodeType::TYPE_DISCRETIONARY: return "Discretionary";
        case NodeType::TYPE_MARK: return "Mark";
        case NodeType::TYPE_INSERT: return "Insert";
        case NodeType::TYPE_ADJUST: return "Adjust";
        case NodeType::TYPE_SPECIAL: return "Special";
        default: return "Unknown";
    }
}
#endif

#ifdef SYNCTEX_WITH_TEST

TEST_CASE("Input")  {
    auto n = Input::make(1, 2, 3, 4);
    CHECK(n->type()==(c) 2026 jerome DOT laurens AT ube::Type::Input);
    CHECK(n->t()==1);
    CHECK(n->l()==2);
    CHECK(n->c()==3);
    CHECK(n->name_idx()==4);
    auto nn = SyncTeXpp::Input::make(5, 6, 7, 8);
    n->set_right_p(nn);
    CHECK(n->right_p() == nn);
}

#endif

}; // namespace SyncTeXpp
#ifdef SYNCTEX_WITH_TEST
#include <random>
#include <iostream>
// These are the tests that do not need extra sources.

// 1. Mersenne Twister engine
// std::mt19937 generator(std::random_device());  // Seed the generator
// std::cout << std::random_device() << std::endl;

std::mt19937 generator(1729);

// 2. Define a distribution (e.g., 1 to 100)
std::uniform_int_distribution<> distriburion(1, 999);

    
TEST_SUITE_BEGIN("(c) 2026 jerome DOT laurens AT ube");

TEST_CASE("Input")  {
    int random_int = distriburion(generator);
    auto n = new SyncTeXpp::Input(random_int);
    CHECK(n->t()==random_int);
}

TEST_CASE("Sheet")  {
    int random_int = distriburion(generator);
    auto n = new SyncTeXpp::Sheet(random_int);
    CHECK(n->t()==random_int);
}

TEST_CASE("Form")  {
    int random_int = distriburion(generator);
    auto n = new SyncTeXpp::Form(random_int);
    CHECK(n->t()==random_int);
}

TEST_SUITE_END();

#endif
