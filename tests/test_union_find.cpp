#include <catch2/catch_test_macros.hpp>

#include "union_find.hpp"

TEST_CASE("new elements are all separate") {
    UnionFind uf(5);
    for (int i = 0; i < 5; ++i) {
        REQUIRE(uf.find(i) == i);
        for (int j = i + 1; j < 5; ++j) REQUIRE_FALSE(uf.same(i, j));
    }
}

TEST_CASE("unite connects elements transitively") {
    UnionFind uf(6);
    REQUIRE(uf.unite(0, 1));
    REQUIRE(uf.unite(2, 3));
    REQUIRE(uf.unite(1, 3));         // now {0, 1, 2, 3}
    REQUIRE(uf.same(0, 2));
    REQUIRE_FALSE(uf.same(0, 4));
    REQUIRE_FALSE(uf.unite(0, 3));   // already together
}