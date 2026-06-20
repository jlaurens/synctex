/**
 * SyncTeXTree.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

 /**
  * @brief For a planar tree
  * 
  * Navigation:
  * - left_p: owner, right_p: owned
  * - above_p: owner, below_p: owned
  * 
  * Class hierarchy:
  * 
  * Node:           the getters
  * └─ Below:       Node with no child
  *    └─ Above:    Node with children (boxes and alike)
  */

#ifndef SYNCTEX_TREE_HPP
#define SYNCTEX_TREE_HPP

#include "SyncTeXNode.hpp"
#include "SyncTeXPrefix.hpp"

namespace SyncTeXpp {

class Below;
using Below_p = std::shared_ptr<Below>;
using Below_w = std::weak_ptr<Below>;
using PairBelow_p = std::pair<Below_p, Below_p>;

class Above;
using Above_p = std::shared_ptr<Above>;
using Above_w = std::weak_ptr<Above>;

class Form;
using Form_p = std::shared_ptr<Form>;

class Sheet;
using Sheet_p = std::shared_ptr<Sheet>;

class VBox;
using VBox_p = std::shared_ptr<VBox>;

class HBox;
using HBox_p = std::shared_ptr<HBox>;

class VBox;
using VBox_p = std::shared_ptr<VBox>;

struct Bi_s {
    Below_p p;
    int i;
    Bi_s(
        Below_p p = nullptr,
        int i = std::numeric_limits<int>::max()
    ): p(p), i(i) {}
};

struct lr_Bi_s {
    Bi_s l;
    Bi_s r;
    lr_Bi_s( Bi_s l=Bi_s(), Bi_s r=Bi_s() ): l(l), r(r) {}
};

class Below: virtual public Node
{
    public:
    Below(const Below&) = delete;
    Below& operator=(const Below&) = delete;
    Below(Below&&) = delete;
    Below& operator=(Below&&) = delete;
    virtual ~Below() = default;

    protected:
    Below_w _left_w;
    Below_p _right_p;
    Above_w _above_w;

    int _t = 0;

    Below( int t = 0 ): _t(t) {}

    public:
    int t() const override { return _t; }
    int set_t(int i) { return _t = i; }

    Below_p as_Below_p() { return std::dynamic_pointer_cast<Below>(as_Node_p()); }
    virtual Above_p as_Above_p() { return nullptr; };

    Node_p above_p() const override {  return std::static_pointer_cast<Node>(_above_w.lock()); }
    Above_p get_above_p() const { return _above_w.lock(); }
    /**
     * @brief Set the above node
     * 
     * As we cannot create a Below::Key instance outside
     * the Above and Below hierarchy, compilation fails
     * when trying to call `set_above_p`anywhere else.
     * 
     * @param p The new above node
     * @param Key
     * @return And Above shared pointer
     */
     struct Key {
        private:
        Key() = default;
        friend class Below;
        friend class Above;
    };

    Above_p set_above_p(Above_p p, const Key &key, bool update = false);

    virtual void aboveDidChange(Above_p) {}
    virtual void leftDidChange(Below_p) {}

    Node_p right_p() const override { return std::static_pointer_cast<Node>(_right_p); }
    Below_p get_right_p() const { return _right_p; }

    /**
     * @brief Set the right node.
     * 
     * One shot setter.
     * Do nothing operation when
     * - There is already a right node
     * - <p> is nullptr
     * - <p> is not picked by another node (Sheets are not)
     * 
     * @param p will be reset on return, use the return value.
     * @return the new right node
     */
    Below_p set_right_p(Below_p p) {
        if (!_right_p && p && p.get() != this) {
            if (p->picked()) {
                _right_p = std::move(p);
                _right_p->set_left_p(as_Below_p());
                _right_p->set_above_p(get_above_p(), Key(), true);
            } else if (p->_left_w.expired() && _left_w.expired() && _above_w.expired()) {
                _right_p = std::move(p);
                _right_p->set_left_p(as_Below_p());
                set_above_p(_right_p->get_above_p(), Key(), false);
            }
        }
        return _right_p;
    }

    Node_p left_p() const override { return std::static_pointer_cast<Node>(_left_w.lock()); }
    Below_p get_left_p() const { return _left_w.lock(); }
    protected:
    Below_p set_left_p(Below_p p) {
        if (!_left_w.lock()) {
            if (p && (p->_right_p.get() == this)) {
                _left_w = p;
                leftDidChange(p);
            }
        }
        return _left_w.lock();
    }
    public:
    int level() const override;

    virtual Form_p above_form_p() {
        return nullptr;
        // auto N_p = shared_from_this()->above_p();
        // while (N_p && N_p->type() != Type::Form) {
        //     N_p = N_p->above_p();
        // }
        // /*  exit the while loop either when N_p is nullptr or N_p is a form */
        // return std::dynamic_pointer_cast<Form>(N_p);
    }

    virtual Sheet_p above_sheet_p() {
        return nullptr;
        // auto N_p = shared_from_this()->above_p();
        // while (N_p && N_p->type() != Type::Sheet) {
        //     N_p = N_p->above_p();
        // }
        // /*  exit the while loop either when N_p is nullptr or N_p is a form */
        // return std::dynamic_pointer_cast<Sheet>(N_p);
    }

    virtual bool picked() const { return _above_w.expired() && _left_w.expired() && !right_p(); }

    virtual char prefix() const { return Prefix::NONE; };

    bool is_above() const override { return false; }

    virtual bool isRef()   const { return false; }

    virtual HBox_p asHBox_p() { return nullptr; }
    virtual VBox_p asVBox_p() { return nullptr; }
    
    virtual int mean_l() const  { return l(); }
    virtual int weight() const { return 1; }

    virtual Below_p get_below_p() const { return nullptr; }

    virtual Below_p get_next_p() const;

    /**
     * @brief The deepest container that contains the hit
     * 
     * @param hit 
     * @return Node_p 
     */
    virtual Bi_s closest_container_below(const hv_s &hit) const {
        return Bi_s();
    }
    virtual Bi_s closest_container(const hv_s &hv) {
        return Bi_s();
    }
    virtual Bi_s closest_deep_below_v2(const hv_s &hv) const {
        return Bi_s();
    }
    virtual Bi_s eq_closest_below_v2(const hv_s &hv) const {
        return closest_deep_below_v2(hv);
    }
    virtual lr_Bi_s eq_get_closest_below_in_box_v2(const hv_s &hv) const {
        return lr_Bi_s();
    }    
    /*  Compares the locations of the hit point with the locations of
    *  the various nodes contained in the box.
    *  As it is an horizontal box, we only compare horizontal coordinates.
    */
    virtual lr_Bi_s eq_get_closest_below_in_hbox_v2(hv_s hv) const
    {
        return lr_Bi_s();
    }
    virtual lr_Bi_s eq_get_closest_below_in_vbox_v2(const hv_s &hit) const
    {
        return lr_Bi_s();
    }
    lr_Bi_s eq_get_closest_below_in_box_v2(const hv_s &hit)
    {
        if (type() == Node::Type::HBox || type() == Node::Type::ProxyHBox) {
            return eq_get_closest_below_in_hbox_v2(hit);
        } else {
            return eq_get_closest_below_in_vbox_v2(hit);
        }
        return lr_Bi_s();
    }

};

class Above: public Below
{
    public:
    Above(const Above&) = delete;
    Above& operator=(const Above&) = delete;
    Above(Above&&) = delete;
    Above& operator=(Above&&) = delete;
    virtual ~Above() = default;

    protected:
    Below_w _rbelow_w;
    Below_p _below_p;

    Above( int t = 0 ): Below(t) {};

    public:

    static Above_p make_p( int t = 0 ) {
        return Above_p(new Above(t));
    }

    Node_p rbelow_p() const final override { return _rbelow_w.lock(); }
    Node_p below_p()  const final override { return _below_p; }

    Below_p get_rbelow() const { return _rbelow_w.lock(); }
    Below_p get_below_p()  const override { return _below_p; }

    Below_p set_rbelow(Below_p p)  {
        if (p) {
            _rbelow_w = p;
        }
        return _rbelow_w.lock();
    }
    /**
     * @brief Set the node below
     * 
     * One shot setter.
     * Do nothing operation when
     * - There is already a below node
     * - <p> is nullptr
     * - <p> is not picked by another node (Sheets are not)
     * If there is already a node below,
     * it will go to the right of <p>, which may not have a right node.
     * 
     * @param p the node candidate, reset on return
     * @return The new node below
     */
    Below_p set_below_p(Below_p p);
    bool is_above() const override final { return true; }
    Above_p as_Above_p() override;

    public:

    bool picked() const override { return !below_p() && Below::picked(); }

    virtual char postfix() const { return '\0'; };

    virtual int set_mean_l(int) { return l(); }
    virtual int set_weight(int) { return 1; }

    Below_p get_next_p() const override {
        auto p = get_right_p();
        if (p) return p;
        p = get_right_p();
        if (p) return p;
        auto pp = get_above_p();
        while (pp) {
            if((p = pp->get_right_p())) break;
            pp = pp->get_above_p();
        }
        return p;
    }
    virtual PairBelow_p make_bdries() {
        return {nullptr, nullptr};
    }
    virtual void fit_to_wnes_V(const Below_p &) {}

};

inline int Below::level() const {
    auto p = _above_w.lock();
    return p ? p->level()+1 : 0;
}

inline Below_p Below::get_next_p() const {
    auto p = get_right_p();
    if (p) return p;
    auto pp = get_above_p();
    while (pp) {
        if((p = pp->get_right_p())) break;
        pp = pp->get_above_p();
    }
    return p;
}

inline Above_p Below::set_above_p(Above_p p, const Key &, bool update) {
    if (p && !_above_w.lock()) {
        _above_w = p;
        if (update) p->set_rbelow(as_Below_p());
        aboveDidChange(p);
    }
    return _above_w.lock();
}

inline Below_p Above::set_below_p(Below_p p) {
    if (p && p.get() != this && p->picked()) {
        if (_below_p != p) {
            if (_below_p) {
                p->set_right_p(_below_p);
                _below_p = std::move(p);
            } else {
                _below_p = std::move(p);
                _below_p->set_above_p(as_Above_p(), Key(), true);
            }
        }
    }
    return _below_p;
}

inline Above_p Above::as_Above_p() {
    return std::dynamic_pointer_cast<Above>(as_Node_p());
}
inline std::ostream& operator<<(std::ostream& os, const Below_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}
inline std::ostream& operator<<(std::ostream& os, const Above_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

};

#endif // SYNCTEX_TREE_HPP