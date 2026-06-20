/**
 * SyncTeXNode.cpp
 * 
 * This file is part of the SyncTeX library.
 * It implements the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include <limits>
#include <cstring>
#include <vector>
#include "SyncTeXBox.hpp"

namespace SyncTeXpp {

/**
 *  Get the child closest to the hit point.
 *  - parameter: hit point
 *  - parameter: containing node
 *  - returns: the child and the distance to the hit point.
 *      SYNCTEX_ND_0 if the parameter node has no children.
 *  - note: recursive call.
 */
// was __synctex_closest_deep_child_v2
Bi_s Box::closest_deep_below_v2(const hv_s &hv) const
{
    auto p = get_below_p();
    if (p) {
        int min_level = level();
        auto best = p->distance_to(hv);
        Below_p best_p = p;
        while((p = p->get_next_p()) && p->level()>= min_level) {
            if (!p->is_above()) {
                int r = std::abs(p->distance_to(hv));
                if (r < best) {
                    best_p = p;
                } else if (r == best) {
                    if (p->level() > best_p->level()) {
                        best_p = p;
                    }
                }
            }
        }
    }
    return Bi_s();
}

/**
 *  Return the closest child.
 *  - parameter: a pointer to the hit point,
 *  - parameter: the container
 *  - return: SYNCTEX_ND_0 if node has no child,
 *      the __synctex_closest_deep_child_v2 otherwise.
 */
Bi_s Box::eq_closest_below_v2(const hv_s &hv) const
{
    auto bi = closest_deep_below_v2(hv);//__synctex_closest_deep_child_v2
    if (bi.p && bi.p->isBox()) {
        Below_p below_p;
        if ((below_p = bi.p->get_below_p())) {
            Bi_s best = {below_p, below_p->distance_to(hv)};
            while ((below_p = below_p->get_right_p())) {
                int d = below_p->distance_to(hv);
                if (d < best.i) {
                    best = {below_p, d};
                } else if ((d == best.i) && (below_p->type() != Type::Kern)) {
                    best.p = below_p;
                }
            }
            return best;
        }
    }
    return bi;
}

// was __synctex_eq_get_closest_children_in_hbox_v3
lr_Bi_s Box::eq_get_closest_below_in_box(const hv_s &hv) const
{
    return lr_Bi_s();
}

lr_Bi_s Box::eq_get_closest_below_in_box_v3(const hv_s &hv) const
{
    Bi_s bi;
    Bi_s left;
    Bi_s right;
    if ((bi.p = get_below_p())) {
        do {
            bi.i = Geo::h_relative_to(bi.p->wnes(), hv);
            if (bi.i > 0) {
                /*  node is to the right of the hit point.
                 *  We compare node and the previously recorded one, through the recorded distance.
                 *  If the nodes have the same tag, prefer the one with the smallest line number,
                 *  if the nodes also have the same line number, prefer the one with the smallest column. */
                if (right.i > bi.i) {
                    right = bi;
                } else if (right.i == bi.i && right.p) {
                    if ((right.p->t() == bi.p->t())
                        && (right.p->l() > bi.p->l()
                            || (right.p->l() == bi.p->l()
                                && right.p->c() > bi.p->c()))) {
                                right = bi;
                            }
                }
            } else if (bi.i == 0) {
                /*  hit point is inside node. */
                left = bi;
            } else { /*  here bi.i < 0, the hit point is to the right of node */
                bi.i = -bi.i;
                if (left.i > bi.i) {
                    left = bi;
                } else if (left.i == bi.i && left.p) {
                    if (left.p->t() == bi.p->t()
                        && (left.p->l() > bi.p->l()
                            || (left.p->l() == bi.p->l()
                                && left.p->c() > bi.p->c()))) {
                                left = bi;
                            }
                }
            }
        } while((bi.p = bi.p->get_below_p()));
        if (left.p) {
            /*  the left node is new, try to narrow the result */
            if ((bi = left.p->closest_container_below(hv)).p) {
                left.p = bi.p;
            }
            if((bi = left.p->eq_closest_below_v2(hv)).p) {
                left.p = bi.p;
            }
        }
        if (right.p) {
            /*  the right node is new, try to narrow the result */
            if ((bi = right.p->closest_container_below(hv)).p) {
                right.p = bi.p;
            }
            if((bi = right.p->eq_closest_below_v2(hv)).p) {
                right.p = bi.p;
            }
        }
    }
    return { left, right };
}

lr_Bi_s Box::eq_get_closest_below_in_vbox_v2(const hv_s &hv) const
{
    Bi_s bi;
    Bi_s left;
    Bi_s right;
    if ((bi.p = get_below_p())) {
        do {
            bi.i = Geo::v_relative_to(bi.p->wnes(), hv);
            /*  this is what makes the difference with the h version above */
            if (bi.i > 0) {
                /*  node is to the top of the hit point (below because TeX is oriented from top to bottom.
                 *  We compare node and the previously recorded one, through the recorded distance.
                 *  If the nodes have the same tag, prefer the one with the smallest line number,
                 *  if the nodes also have the same line number, prefer the one with the smallest column. */
                if (right.i > bi.i) {
                    right = bi;
                } else if (right.i == bi.i && right.p) {
                    if (right.p->t() == bi.p->t()
                        && (right.p->l() > bi.p->l()
                            || (right.p->l() == bi.p->l()
                                && right.p->c() > bi.p->c()))) {
                        right = bi;
                    }
                }
            } else if (bi.i == 0) {
                left = bi;
            } else { /*  here bi < 0 */
                bi.i = -bi.i;
                if (left.i > bi.i) {
                    left = bi;
                } else if (left.i == bi.i && left.p) {
                    if (left.p->t() == bi.p->t()
                        && (left.p->l() > bi.p->l()
                            || (left.p->l() == bi.p->l()
                                && left.p->c() > bi.p->c()))) {
                        left = bi;
                    }
                }
            }
        } while ((bi.p = bi.p->get_right_p()));
        if (left.p) {
            if ((bi = left.p->closest_container_below(hv)).p) {
                left.p = bi.p;
            }
            if ((bi = left.p->eq_closest_below_v2(hv)).p) {
                left.p = bi.p;
            }
        }
        if (right.p) {
            if ((bi = right.p->closest_container_below(hv)).p) {
                right.p = bi.p;
            }
            if ((bi = right.p->eq_closest_below_v2(hv)).p) {
                right.p = bi.p;
            }
        }
    }
    return {left, right};
}

Bi_s Box::closest_container(const hv_s &hv)
{
    auto below_p = get_below_p();
    if (get_below_p()) {
        /*  Non void hbox, vbox, box proxy or form ref */
        /*  We go deep first because some boxes have 0 dimensions
            *  despite they do contain some black material.
            */
        do {
            Bi_s deep = below_p->closest_container(hv);
            if (deep.p) {
                /*  One of the children contains the hv-> */
                return deep;
            }
        } while ((below_p = below_p->get_right_p()));
        /*  For vboxes we try to use some node inside.
            *  Walk through the list of siblings until we find the closest one.
            *  Only consider siblings with children inside. */
        if (type() == Type::VBox || type() == Type::ProxyVBox) {
            if ((below_p = get_below_p())) {
                Bi_s best;
                do {
                    if (below_p->below_p()) {
                        int d = below_p->distance_to(hv);
                        if (d < best.i) {
                            best = {below_p, d};
                        }
                    }
                } while ((below_p = below_p->get_right_p()));
                if (best.p) {
                    return best;
                }
            }
        }
        /*  is the hv point inside the box? */
        if (contains(hv)) {
            return {
                std::dynamic_pointer_cast<Below>(shared_from_this()), 0
            };
        }
    }
    return Bi_s();
}
// was __synctex_eq_get_closest_children_in_hbox_v2
lr_Bi_s HBox::eq_get_closest_below_in_hbox_v2(hv_s hv) const
{
    Bi_s bi;
    lr_Bi_s lr_bi;
    if ((bi.p = get_below_p())) {
        do {
            bi.i = Geo::h_relative_to(bi.p->wnes(), hv);
            if (bi.i > 0) {
                /*  node is to the right of the hit point.
                *  We compare node and the previously recorded one, through the recorded distance.
                *  If the nodes have the same tag, prefer the one with the smallest line number,
                *  if the nodes also have the same line number, prefer the one with the smallest column. */
                if (lr_bi.r.i > bi.i) {
                    lr_bi.r = bi;
                } else if (lr_bi.r.i == bi.i && lr_bi.r.p) {
                    if (lr_bi.r.p->t() == bi.p->t()
                        && (lr_bi.r.p->l() > bi.p->l()
                            || (lr_bi.r.p->l() == bi.p->l()
                                && lr_bi.r.p->c() > bi.p->c()))) {
                        lr_bi.r = bi;
                    }
                }
            } else if (bi.i == 0) {
                /*  hit point is inside node. */
                if (bi.p->below_p()) {
                    return bi.p->eq_get_closest_below_in_box_v2(hv);
                }
                lr_bi.l = bi;
            } else { /*  here bi.i < 0, the hit point is to the right of node */
                bi.i = -bi.i;
                if (lr_bi.l.i > bi.i) {
                    lr_bi.l = bi;
                } else if (lr_bi.l.i == bi.i && lr_bi.l.p) {
                    if (lr_bi.l.p->t() == bi.p->t()
                        && (lr_bi.l.p->l() > bi.p->l()
                            || (lr_bi.l.p->l() == bi.p->l()
                                && lr_bi.l.p->c() > bi.p->c()))) {
                        lr_bi.l = bi;
                    }
                }
            }
        } while ((bi.p = bi.p->get_right_p()));
        if (lr_bi.l.p) {
            /*  the left node is new, try to narrow the result */
            if ((bi = lr_bi.l.p->closest_container(hv)).p) {
                lr_bi.l = bi;
            }
            if ((bi = lr_bi.l.p->closest_deep_below_v2(hv)).p) {
                lr_bi.l.p = bi.p;
            }
        }
        if (lr_bi.r.p) {
            /*  the right node is new, try to narrow the result */
            if ((bi = lr_bi.r.p->closest_container(hv)).p) {
                lr_bi.r = bi;
            }
            if ((bi = lr_bi.r.p->closest_deep_below_v2(hv)).p) {
                lr_bi.r.p = bi.p;
            }
        }
    }
    return lr_bi;
}


#if 0
Node* Node::set_friend(Node* friend_node) {
    if (tree_model_.friend_idx >= 0) {
        Node* old_friend = data_[tree_model_.friend_idx].as_node;
        data_[tree_model_.friend_idx].as_node = friend_node;
        return old_friend;
    }
    return nullptr;
}

// --- Node Traversal ---


// --- Parent Sheet/Form ---


// --- Utility ---

const char* Node::isa() const {
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

}; // namespace SyncTeXpp
