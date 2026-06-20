/**
 * Node.hpp
 * 
 * This file is part of the SyncTeX library.
 * It implements the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#ifndef SYNCTEX_TEST_NODE_HPP
#define SYNCTEX_TEST_NODE_HPP

#include "SyncTeXNode.hpp"

#define SYNCTEX_CHECK_ALR(P, A, L, R)   \
CHECK_EQ(A, P->above_p());                \
CHECK_EQ(L, P->left_p());                 \
CHECK_EQ(R, P->right_p());

#define SYNCTEX_CHECK_ALBRr(P, A, L, B, R, r)   \
CHECK_EQ(A, P->above_p());                        \
CHECK_EQ(L, P->left_p());                         \
CHECK_EQ(B, P->below_p());                        \
CHECK_EQ(R, P->right_p());                        \
CHECK_EQ(r, P->rbelow_p());

#endif // SYNCTEX_TEST_NODE_HPP