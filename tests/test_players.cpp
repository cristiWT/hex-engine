#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

#include "board.hpp"
#include "players/player.hpp"

namespace {  // helpers visible only in this file

// Plays one full game with `player` choosing every move (both colors) and
// returns the sequence of moves it chose.
std::vector<int> self_play_moves(Player& player, int n, bool is_swap_on) {
    Board board(n, is_swap_on);
    std::vector<int> moves;
    while (board.winner() == Color::empty) {
        const int move = player.choose_move(board);
        REQUIRE(board.is_legal(move));
        moves.push_back(move);
        board.play(move);
    }
    return moves;
}

}  // namespace

TEST_CASE("random player only ever returns legal moves, swap on or off") {
    for (bool swap : {false, true}) {
        auto p = make_random_player(99);
        for (int n = 1; n <= 7; ++n) {
            for (int game = 0; game < 50; ++game) {
                self_play_moves(*p, n, swap);  // REQUIRE inside checks legality
            }
        }
    }
}

TEST_CASE("random player: same seed, same game; different seed, different game") {
    auto a1 = make_random_player(2024);
    auto a2 = make_random_player(2024);
    auto b = make_random_player(2025);
    const std::vector<int> game_a1 = self_play_moves(*a1, 9, true);
    const std::vector<int> game_a2 = self_play_moves(*a2, 9, true);
    const std::vector<int> game_b = self_play_moves(*b, 9, true);
    REQUIRE(game_a1 == game_a2);
    REQUIRE(game_a1 != game_b);
}

TEST_CASE("random player sometimes chooses the swap when it is offered") {
    // On a 2x2 board after red's first move, blue has 3 cells + swap: a uniform
    // random player picks the swap 1/4 of the time. Over 200 tries, never
    // picking it (probability (3/4)^200) means the swap move is never offered.
    auto p = make_random_player(7);
    int swaps = 0;
    for (int i = 0; i < 200; ++i) {
        Board board(2, true);
        board.play(board.index(0, 0));
        if (p->choose_move(board) == 2 * 2) ++swaps;  // swap index is n*n
    }
    REQUIRE(swaps > 0);
}
