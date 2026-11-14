#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "arena.hpp"

using Catch::Matchers::WithinAbs;

// Reference values computed independently with the textbook formula
//   center = (p + z²/2n) / (1 + z²/n)
//   half   = z * sqrt(p(1-p)/n + z²/4n²) / (1 + z²/n)

TEST_CASE("wilson: 100/100 is about [0.963, 1.0]") {
    auto [lo, hi] = wilson(100, 100);
    CHECK_THAT(lo, WithinAbs(0.96301, 1e-4));
    CHECK_THAT(hi, WithinAbs(1.0, 1e-9));
}

TEST_CASE("wilson: 55/100 is about [0.452, 0.644]") {
    auto [lo, hi] = wilson(55, 100);
    CHECK_THAT(lo, WithinAbs(0.45244, 1e-4));
    CHECK_THAT(hi, WithinAbs(0.64386, 1e-4));
}

TEST_CASE("wilson: 0 wins has a lower bound of 0 (and a nonzero upper bound)") {
    // Note: floating point can give something like -2.8e-17 here instead of an
    // exact 0, so compare with a tolerance rather than `lo >= 0`.
    for (int n : {1, 10, 100, 1000}) {
        auto [lo, hi] = wilson(0, n);
        CHECK_THAT(lo, WithinAbs(0.0, 1e-12));
        // Upper bound for 0/n has the closed form z² / (n + z²).
        const double z2 = 1.96 * 1.96;
        CHECK_THAT(hi, WithinAbs(z2 / (n + z2), 1e-9));
    }
}

TEST_CASE("wilson: n == 0 says nothing is known, [0, 1]") {
    auto [lo, hi] = wilson(0, 0);
    CHECK(lo == 0.0);
    CHECK(hi == 1.0);
}

TEST_CASE("wilson: interval stays inside [0, 1] and contains the observed rate") {
    constexpr double eps = 1e-12;
    for (int n = 1; n <= 60; ++n) {
        for (int w = 0; w <= n; ++w) {
            auto [lo, hi] = wilson(w, n);
            const double p = static_cast<double>(w) / n;
            INFO("w = " << w << ", n = " << n << ", interval = [" << lo << ", " << hi << "]");
            REQUIRE(lo >= -eps);
            REQUIRE(hi <= 1.0 + eps);
            REQUIRE(lo <= p + eps);
            REQUIRE(p <= hi + eps);
        }
    }
}

TEST_CASE("wilson: w wins is the mirror image of n - w wins") {
    // Swapping who we call "the winner" flips the interval around 0.5.
    for (int n = 1; n <= 40; ++n) {
        for (int w = 0; w <= n; ++w) {
            auto [lo, hi] = wilson(w, n);
            auto [lo_m, hi_m] = wilson(n - w, n);
            INFO("w = " << w << ", n = " << n);
            REQUIRE_THAT(lo, WithinAbs(1.0 - hi_m, 1e-9));
            REQUIRE_THAT(hi, WithinAbs(1.0 - lo_m, 1e-9));
        }
    }
}

TEST_CASE("wilson: more games give a narrower interval, larger z a wider one") {
    auto [lo100, hi100] = wilson(50, 100);
    auto [lo1000, hi1000] = wilson(500, 1000);
    CHECK(hi1000 - lo1000 < hi100 - lo100);

    auto [lo95, hi95] = wilson(50, 100, 1.96);
    auto [lo99, hi99] = wilson(50, 100, 2.576);
    CHECK(hi99 - lo99 > hi95 - lo95);
}
