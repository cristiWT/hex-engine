#include <catch2/catch_test_macros.hpp>

TEST_CASE("sanitizer check") {
    std::vector<int> a(10);
    int x = a[10];
    REQUIRE(x == x);
}