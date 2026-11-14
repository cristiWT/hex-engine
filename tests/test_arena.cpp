#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <memory>
#include <string>

#include "arena.hpp"
#include "board.hpp"
#include "players/player.hpp"

namespace {  // helpers visible only in this file

// Wraps another player and checks, on every move, that play_game is driving
// the game correctly. Catch2 assertions work fine inside helper functions.
class CheckingPlayer : public Player {
public:
    CheckingPlayer(Player& inner, Color expected_color)
        : inner_(inner), expected_color_(expected_color) {}

    int choose_move(const Board& board) override {
        ++moves_;
        // play_game must stop as soon as someone has won...
        REQUIRE(board.winner() == Color::empty);
        // ...and must ask the player whose turn it actually is.
        REQUIRE(board.to_move() == expected_color_);
        const int move = inner_.choose_move(board);
        REQUIRE(board.is_legal(move));
        return move;
    }

    std::string player_type() const override { return "checking " + inner_.player_type(); }

    int moves() const { return moves_; }
    void reset() { moves_ = 0; }

private:
    Player& inner_;
    Color expected_color_;
    int moves_ = 0;
};

// game_run_time is wall-clock time, so it never matches between runs.
// Compare only the deterministic fields.
bool same_counts(const MatchResults& x, const MatchResults& y) {
    return x.nr_games == y.nr_games && x.a_wins == y.a_wins &&
           x.a_games_as_red == y.a_games_as_red && x.a_wins_as_red == y.a_wins_as_red &&
           x.red_wins == y.red_wins;
}

// Fresh players every call: reusing player objects would continue their RNG
// streams, so the second run would NOT start from the same state.
MatchResults random_match(std::uint64_t seed_a, std::uint64_t seed_b, int nr_games,
                          int board_size, bool is_swap_on) {
    auto a = make_random_player(seed_a);
    auto b = make_random_player(seed_b);
    return run_match(*a, *b, nr_games, board_size, is_swap_on);
}

// Invariants that must hold for ANY match, whatever the players.
void check_bookkeeping(const MatchResults& r, int nr_games) {
    INFO("a_wins = " << r.a_wins << ", a_games_as_red = " << r.a_games_as_red
         << ", a_wins_as_red = " << r.a_wins_as_red << ", red_wins = " << r.red_wins);
    REQUIRE(r.nr_games == nr_games);

    // Colors alternate, so A is red in half the games (rounded either way).
    REQUIRE((r.a_games_as_red == nr_games / 2 || r.a_games_as_red == (nr_games + 1) / 2));

    const int a_games_as_blue = nr_games - r.a_games_as_red;
    const int a_wins_as_blue = r.a_wins - r.a_wins_as_red;
    REQUIRE(0 <= r.a_wins_as_red);
    REQUIRE(r.a_wins_as_red <= r.a_games_as_red);
    REQUIRE(0 <= a_wins_as_blue);
    REQUIRE(a_wins_as_blue <= a_games_as_blue);

    // Red wins = (A won as red) + (B won as red), and B is red exactly when
    // A is blue, so B's wins as red = A's losses as blue.
    const int b_wins_as_red = a_games_as_blue - a_wins_as_blue;
    REQUIRE(r.red_wins == r.a_wins_as_red + b_wins_as_red);

    REQUIRE(r.game_run_time >= 0.0);
}

}  // namespace

TEST_CASE("play_game: every game ends with one winner within N*N (+1 with swap) moves") {
    for (bool swap : {false, true}) {
        for (int n = 1; n <= 8; ++n) {
            auto red_inner = make_random_player(1000 + n);
            auto blue_inner = make_random_player(2000 + n);
            CheckingPlayer red(*red_inner, Color::red);
            CheckingPlayer blue(*blue_inner, Color::blue);

            for (int game = 0; game < 100; ++game) {
                red.reset();
                blue.reset();
                const int winner = play_game(red, blue, n, swap);
                const int total = red.moves() + blue.moves();

                INFO("n = " << n << ", swap = " << swap << ", game " << game
                     << ", moves = " << total);
                REQUIRE((winner == 0 || winner == 1));
                // Every cell filled once, plus the swap move which places no new stone.
                REQUIRE(total <= n * n + (swap ? 1 : 0));
                // The winner needs at least n stones, so at least 2n - 1 moves were played.
                REQUIRE(total >= 2 * n - 1);
                // Red moves first and players alternate.
                REQUIRE((red.moves() == blue.moves() || red.moves() == blue.moves() + 1));
            }
        }
    }
}

TEST_CASE("run_match: on a 1x1 board red always wins, so every count is known exactly") {
    // The first (red) stone on a 1x1 board touches both red edges: instant win.
    for (int nr_games : {1, 2, 7, 10}) {
        auto a = make_random_player(1);
        auto b = make_random_player(2);
        const MatchResults r = run_match(*a, *b, nr_games, 1, true);

        INFO("nr_games = " << nr_games);
        check_bookkeeping(r, nr_games);
        REQUIRE(r.red_wins == nr_games);
        REQUIRE(r.a_wins == r.a_games_as_red);
        REQUIRE(r.a_wins_as_red == r.a_games_as_red);
    }
}

TEST_CASE("run_match: bookkeeping invariants hold on real random games") {
    for (bool swap : {false, true}) {
        for (int nr_games : {1, 2, 99, 100}) {
            INFO("swap = " << swap << ", nr_games = " << nr_games);
            check_bookkeeping(random_match(7, 8, nr_games, 5, swap), nr_games);
        }
    }
}

TEST_CASE("run_match: random vs random, the 95% interval for A's win rate contains 50%") {
    // A and B are the same algorithm and swap colors every game, so A's true
    // win rate is exactly 50%. (Red's win rate is NOT 50% under random play:
    // red moves first and gets more stones, so don't test red_wins here.)
    //
    // Caveat: with a fixed seed this is deterministic. A correct arena would
    // still fail for ~5% of seeds, so if you change the seed and this fails
    // once, try a few seeds before suspecting a bug.
    constexpr int nr_games = 2000;
    for (bool swap : {false, true}) {
        const MatchResults r = random_match(12345, 54321, nr_games, 7, swap);
        auto [lo, hi] = wilson(r.a_wins, r.nr_games);
        INFO("swap = " << swap << ", A won " << r.a_wins << "/" << r.nr_games
             << ", interval = [" << lo << ", " << hi << "]");
        REQUIRE(r.nr_games == nr_games);
        REQUIRE(lo <= 0.5);
        REQUIRE(0.5 <= hi);
    }
}

TEST_CASE("run_match: same seeds give the same result, different seeds a different one") {
    const MatchResults first = random_match(42, 43, 200, 7, true);
    const MatchResults again = random_match(42, 43, 200, 7, true);
    REQUIRE(same_counts(first, again));

    // Deterministic too (fixed seeds); a collision on all counts over 200
    // games is astronomically unlikely, it would point to the seed being ignored.
    const MatchResults other = random_match(1000, 1001, 200, 7, true);
    REQUIRE_FALSE(same_counts(first, other));
}
