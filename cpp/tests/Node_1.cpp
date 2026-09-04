/**
 * SyncTeXNode.cpp
 * 
 * This file is part of the SyncTeX library.
 * It implements the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include "Node.hpp"
#include "SyncTeXGeo.hpp"

using namespace SyncTeXpp;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE_BEGIN("Geo");


#define SYNCTEX_CHECK(H, V, VALUE)  \
CHECK_EQ(Geo::h_relative_to(wnes, {H, V}), VALUE)

TEST_CASE("static h_relative_to")  {
    CHECK(1 == 1);
    auto hv = hv_s({0,0});
    wnes_s wnes;
    CHECK_EQ(Geo::h_relative_to(wnes, hv), 0);
    SYNCTEX_CHECK(0,0,0);
    wnes = {-10, -20, 10, 20};

    SYNCTEX_CHECK(-12,-23, 2);
    SYNCTEX_CHECK(-10,-23, 0);
    SYNCTEX_CHECK(  0,-23, 0);
    SYNCTEX_CHECK( 10,-23, 0);
    SYNCTEX_CHECK( 12,-23,-2);

    SYNCTEX_CHECK(-12,-20, 2);
    SYNCTEX_CHECK(-10,-20, 0);
    SYNCTEX_CHECK(  0,-20, 0);
    SYNCTEX_CHECK( 10,-20, 0);
    SYNCTEX_CHECK( 12,-20,-2);

    SYNCTEX_CHECK(-12,  0, 2);
    SYNCTEX_CHECK(-10,  0, 0);
    SYNCTEX_CHECK(  0,  0, 0);
    SYNCTEX_CHECK( 10,  0, 0);
    SYNCTEX_CHECK( 12,  0,-2);

    SYNCTEX_CHECK(-12, 20, 2);
    SYNCTEX_CHECK(-10, 20, 0);
    SYNCTEX_CHECK(  0, 20, 0);
    SYNCTEX_CHECK( 10, 20, 0);
    SYNCTEX_CHECK( 12, 20,-2);

    SYNCTEX_CHECK(-12, 24, 2);
    SYNCTEX_CHECK(-10, 24, 0);
    SYNCTEX_CHECK(  0, 24, 0);
    SYNCTEX_CHECK( 10, 24, 0);
    SYNCTEX_CHECK( 12, 24,-2);
}

#undef SYNCTEX_CHECK
#define SYNCTEX_CHECK(H, V, VALUE)  \
CHECK_EQ(Geo::v_relative_to(wnes, {H, V}), VALUE)

TEST_CASE("static v_relative_to")  {
    CHECK(1 == 1);
    auto hv = hv_s({0,0});
    wnes_s wnes;
    CHECK_EQ(Geo::v_relative_to(wnes, hv), 0);
    SYNCTEX_CHECK(0,0,0);
    wnes = {-10, -20, 10, 20};

    SYNCTEX_CHECK(-12,-23, 3);
    SYNCTEX_CHECK(-10,-23, 3);
    SYNCTEX_CHECK(  0,-23, 3);
    SYNCTEX_CHECK( 10,-23, 3);
    SYNCTEX_CHECK( 12,-23, 3);

    SYNCTEX_CHECK(-12,-20, 0);
    SYNCTEX_CHECK(-10,-20, 0);
    SYNCTEX_CHECK(  0,-20, 0);
    SYNCTEX_CHECK( 10,-20, 0);
    SYNCTEX_CHECK( 12,-20, 0);

    SYNCTEX_CHECK(-12,  0, 0);
    SYNCTEX_CHECK(-10,  0, 0);
    SYNCTEX_CHECK(  0,  0, 0);
    SYNCTEX_CHECK( 10,  0, 0);
    SYNCTEX_CHECK( 12,  0, 0);

    SYNCTEX_CHECK(-12, 20, 0);
    SYNCTEX_CHECK(-10, 20, 0);
    SYNCTEX_CHECK(  0, 20, 0);
    SYNCTEX_CHECK( 10, 20, 0);
    SYNCTEX_CHECK( 12, 20, 0);

    SYNCTEX_CHECK(-12, 24,-4);
    SYNCTEX_CHECK(-10, 24,-4);
    SYNCTEX_CHECK(  0, 24,-4);
    SYNCTEX_CHECK( 10, 24,-4);
    SYNCTEX_CHECK( 12, 24,-4);
}

#undef SYNCTEX_CHECK
#define SYNCTEX_CHECK(H, V, VALUE_H, VALUE_V)  \
CHECK_EQ(Geo::relative_to(wnes, {H, V}), hv_s(VALUE_H, VALUE_V))

TEST_CASE("static relative_to")  {
    CHECK(1 == 1);
    auto hv = hv_s({0,0});
    wnes_s wnes;
    CHECK_EQ(Geo::relative_to(wnes, hv), 0);
    SYNCTEX_CHECK(0,0,0,0);

    wnes = {-10, -20, 10, 20};

    SYNCTEX_CHECK(-12,-23, 2, 3);
    SYNCTEX_CHECK(-10,-23, 0, 3);
    SYNCTEX_CHECK(  0,-23, 0, 3);
    SYNCTEX_CHECK( 10,-23, 0, 3);
    SYNCTEX_CHECK( 12,-23,-2, 3);

    SYNCTEX_CHECK(-12,-20, 2, 0);
    SYNCTEX_CHECK(-10,-20, 0, 0);
    SYNCTEX_CHECK(  0,-20, 0, 0);
    SYNCTEX_CHECK( 10,-20, 0, 0);
    SYNCTEX_CHECK( 12,-20,-2, 0);

    SYNCTEX_CHECK(-12,  0, 2, 0);
    SYNCTEX_CHECK(-10,  0, 0, 0);
    SYNCTEX_CHECK(  0,  0, 0, 0);
    SYNCTEX_CHECK( 10,  0, 0, 0);
    SYNCTEX_CHECK( 12,  0,-2, 0);

    SYNCTEX_CHECK(-12, 20, 2, 0);
    SYNCTEX_CHECK(-10, 20, 0, 0);
    SYNCTEX_CHECK(  0, 20, 0, 0);
    SYNCTEX_CHECK( 10, 20, 0, 0);
    SYNCTEX_CHECK( 12, 20,-2, 0);

    SYNCTEX_CHECK(-12, 24, 2,-4);
    SYNCTEX_CHECK(-10, 24, 0,-4);
    SYNCTEX_CHECK(  0, 24, 0,-4);
    SYNCTEX_CHECK( 10, 24, 0,-4);
    SYNCTEX_CHECK( 12, 24,-2,-4);
}

#undef SYNCTEX_CHECK
#define SYNCTEX_CHECK(H, V, VALUE)  \
CHECK_EQ(Geo::h_distance_to(wnes, {H, V}), VALUE)

TEST_CASE("static h_distance_to")  {
    CHECK(1 == 1);
    auto hv = hv_s({0,0});
    wnes_s wnes;
    CHECK_EQ(Geo::h_distance_to(wnes, hv), 0);
    SYNCTEX_CHECK(0,0,0);
    wnes = {-10, -20, 10, 20};

    SYNCTEX_CHECK(-12,-23, 2);
    SYNCTEX_CHECK(-10,-23, 0);
    SYNCTEX_CHECK(  0,-23, 0);
    SYNCTEX_CHECK( 10,-23, 0);
    SYNCTEX_CHECK( 12,-23, 2);

    SYNCTEX_CHECK(-12,-20, 2);
    SYNCTEX_CHECK(-10,-20, 0);
    SYNCTEX_CHECK(  0,-20, 0);
    SYNCTEX_CHECK( 10,-20, 0);
    SYNCTEX_CHECK( 12,-20, 2);

    SYNCTEX_CHECK(-12,  0, 2);
    SYNCTEX_CHECK(-10,  0, 0);
    SYNCTEX_CHECK(  0,  0, 0);
    SYNCTEX_CHECK( 10,  0, 0);
    SYNCTEX_CHECK( 12,  0, 2);

    SYNCTEX_CHECK(-12, 20, 2);
    SYNCTEX_CHECK(-10, 20, 0);
    SYNCTEX_CHECK(  0, 20, 0);
    SYNCTEX_CHECK( 10, 20, 0);
    SYNCTEX_CHECK( 12, 20, 2);

    SYNCTEX_CHECK(-12, 24, 2);
    SYNCTEX_CHECK(-10, 24, 0);
    SYNCTEX_CHECK(  0, 24, 0);
    SYNCTEX_CHECK( 10, 24, 0);
    SYNCTEX_CHECK( 12, 24, 2);
}

#undef SYNCTEX_CHECK
#define SYNCTEX_CHECK(H, V, VALUE)  \
CHECK_EQ(Geo::v_distance_to(wnes, {H, V}), VALUE)

TEST_CASE("static v_distance_to")  {
    CHECK(1 == 1);
    auto hv = hv_s({0,0});
    wnes_s wnes;
    CHECK_EQ(Geo::v_distance_to(wnes, hv), 0);
    SYNCTEX_CHECK(0,0,0);
    wnes = {-10, -20, 10, 20};

    SYNCTEX_CHECK(-12,-23, 3);
    SYNCTEX_CHECK(-10,-23, 3);
    SYNCTEX_CHECK(  0,-23, 3);
    SYNCTEX_CHECK( 10,-23, 3);
    SYNCTEX_CHECK( 12,-23, 3);

    SYNCTEX_CHECK(-12,-20, 0);
    SYNCTEX_CHECK(-10,-20, 0);
    SYNCTEX_CHECK(  0,-20, 0);
    SYNCTEX_CHECK( 10,-20, 0);
    SYNCTEX_CHECK( 12,-20, 0);

    SYNCTEX_CHECK(-12,  0, 0);
    SYNCTEX_CHECK(-10,  0, 0);
    SYNCTEX_CHECK(  0,  0, 0);
    SYNCTEX_CHECK( 10,  0, 0);
    SYNCTEX_CHECK( 12,  0, 0);

    SYNCTEX_CHECK(-12, 20, 0);
    SYNCTEX_CHECK(-10, 20, 0);
    SYNCTEX_CHECK(  0, 20, 0);
    SYNCTEX_CHECK( 10, 20, 0);
    SYNCTEX_CHECK( 12, 20, 0);

    SYNCTEX_CHECK(-12, 24, 4);
    SYNCTEX_CHECK(-10, 24, 4);
    SYNCTEX_CHECK(  0, 24, 4);
    SYNCTEX_CHECK( 10, 24, 4);
    SYNCTEX_CHECK( 12, 24, 4);
}

#undef SYNCTEX_CHECK
#define SYNCTEX_CHECK(H, V, VALUE)  \
CHECK_EQ(Geo::distance_to(wnes, {H, V}), VALUE)

TEST_CASE("static distance_to")  {
    CHECK(1 == 1);
    auto hv = hv_s({0,0});
    wnes_s wnes;
    CHECK_EQ(Geo::distance_to(wnes, hv), 0);
    SYNCTEX_CHECK(0,0,0);

    wnes = {-10, -20, 10, 20};

    SYNCTEX_CHECK(-12,-23, 5);
    SYNCTEX_CHECK(-10,-23, 3);
    SYNCTEX_CHECK(  0,-23, 3);
    SYNCTEX_CHECK( 10,-23, 3);
    SYNCTEX_CHECK( 12,-23, 5);

    SYNCTEX_CHECK(-12,-20, 2);
    SYNCTEX_CHECK(-10,-20, 0);
    SYNCTEX_CHECK(  0,-20, 0);
    SYNCTEX_CHECK( 10,-20, 0);
    SYNCTEX_CHECK( 12,-20, 2);

    SYNCTEX_CHECK(-12,  0, 2);
    SYNCTEX_CHECK(-10,  0, 0);
    SYNCTEX_CHECK(  0,  0, 0);
    SYNCTEX_CHECK( 10,  0, 0);
    SYNCTEX_CHECK( 12,  0, 2);

    SYNCTEX_CHECK(-12, 20, 2);
    SYNCTEX_CHECK(-10, 20, 0);
    SYNCTEX_CHECK(  0, 20, 0);
    SYNCTEX_CHECK( 10, 20, 0);
    SYNCTEX_CHECK( 12, 20, 2);

    SYNCTEX_CHECK(-12, 24, 6);
    SYNCTEX_CHECK(-10, 24, 4);
    SYNCTEX_CHECK(  0, 24, 4);
    SYNCTEX_CHECK( 10, 24, 4);
    SYNCTEX_CHECK( 12, 24, 6);
}

// auto g = Geo();
// auto g_p = std::make_unique<Geo>();
// auto n = std::make_shared<Geo>();

TEST_CASE("TRIVIAL")  {
    auto p = std::make_shared<Geo>();
    auto hv = hv_s();
    CHECK(!p->contains(hv));
    CHECK_EQ(p->h_relative_to(hv), 0);
    CHECK_EQ(p->v_relative_to(hv), 0);
    CHECK_EQ(p->relative_to(hv), hv_s(0,0));
}

TEST_CASE("TRIVIAL")  {
    auto p = std::make_shared<Geo>();
    auto hv = hv_s();
    CHECK_EQ(p->h_relative_to(hv), 0);
    CHECK_EQ(p->v_relative_to(hv), 0);
    CHECK_EQ(p->relative_to(hv), hv_s(0,0));
}

TEST_CASE("ONE L")  {
    auto p = std::make_shared<Geo>();
    auto hv = hv_s();
    CHECK(!p->contains(hv));
    CHECK_EQ(p->h_relative_to(hv), 0);
    CHECK_EQ(p->v_relative_to(hv), 0);
    CHECK_EQ(p->relative_to(hv), hv_s(0,0));
}


// TEST_CASE("TRIVIAL")  {
//     CHECK(1 == 1);
// }


TEST_SUITE_END();
