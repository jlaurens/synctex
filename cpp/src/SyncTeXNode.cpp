/**
 * SyncTeXNode.cpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */


/**
 * @brief Nodes to represent SyncTeX data.
 * 
 * Used by a Scanner instance.
 */


#include <memory>

#include "SyncTeXNode.hpp"
#include "SyncTeXUtils.hpp"

namespace SyncTeXpp {

Node_p Node::as_Node_p() { return shared_from_this(); }

Node::Type Node::type() const { return Type::Node; }
std::string Node::type_name() const { return "Node"; }
Node_p Node::above_p()  const { return nullptr; }
Node_p Node::below_p()  const { return nullptr; }
Node_p Node::rbelow_p() const { return nullptr; }
Node_p Node::left_p()   const { return nullptr; }
Node_p Node::right_p()  const { return nullptr; }

Node_p Node::rightmost_p() {
    Node_p node_p = above_p();
    if (node_p && (node_p = node_p->rbelow_p())) {
        return node_p;
    }
    node_p = as_Node_p();
    Node_p right_p;
    do {
        right_p = node_p;
    } while ((node_p = node_p->right_p()));
    return right_p;
}

int Node::level() const { return 0; }

int Node::t() const { return 0; }
int Node::l() const { return 0; }
int Node::c() const { return 0; }

int Node::h()   const { return 0; }
int Node::v()   const { return 0; }

int Node::W() const { return 0; }
int Node::H() const { return 0; }
int Node::D() const { return 0; }

int Node::w()   const { return std::min(h(), h()+W()); }
int Node::e()   const { return std::max(h(), h()+W()); }
int Node::n()   const { return std::min(v()-H(), v()+D()); }
int Node::s()   const { return std::max(v()-H(), v()+D()) ; }
int Node::w_V() const { return w(); }
int Node::n_V() const { return e(); }
int Node::e_V() const { return n(); }
int Node::s_V() const { return s(); }

wnes_s Node::wnes() const { return { w(), n(), e(), s() }; };
wnes_s Node::wnes_V() const { return wnes(); };

we_s Node::we() const { return { w(), e() }; };
ns_s Node::ns() const { return { n(), s() }; };

tlc_s Node::tlc()   const { return {t(), l(), c()}; };
hv_s Node::hv()     const { return {h(), v()}; };
tlc_hv_s Node::tlc_hv()   const { return {t(), l(), c(), h(), v()}; };
tlc_hv_WHD_s Node::tlc_hv_WHD()   const { return {t(), l(), c(), h(), v(), W(), H(), D()}; };

Node_p Node::next_p() const {
  auto p = below_p();
  if (p) return p;
  p = right_p();
  if (p) return p;
  auto pp = above_p();
  while (pp) {
    if((p = pp->right_p())) break;
    pp = pp->above_p();
  }
  return p;
}

bool Node::isBox()      const { return false; }
bool Node::isHBox()     const { return false; }
bool Node::isVBox()     const { return false; }
bool Node::is_below()   const { return false; }
bool Node::is_above()   const { return false; }
bool Node::isForm()     const { return false; }
bool Node::isSheet()    const { return false; }

std::ostream & Node::log(std::ostream &o) const {
  return o << "⟨NONE⟩";
}

std::ostream& operator<<(std::ostream& os, const Node& a) {
    return a.log(os);
}

std::ostream& operator<<(std::ostream& os, const Node_p& p) {
    return p ? os << *p : os << "⟨NONE⟩";
}

int Node::distance_to(const hv_s &hv) const {
    return std::numeric_limits<int>::max();
}


}; // namespace SyncTeXpp
