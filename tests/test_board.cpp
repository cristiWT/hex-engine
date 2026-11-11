#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <queue>
#include <random>
#include <vector>

#include "board.hpp"

namespace {  // helpers visible only in this file

// Slow but obviously correct: BFS from one edge to see if `player` is connected.
bool bfs_connected(const Board& b, Color player) {
    const int n = b.size();
    std::vector<bool> seen(n * n, false);
    std::queue<int> q;
    for (int i = 0; i < n; ++i) {
        const int start = (player == Color::red) ? b.index(0, i) : b.index(i, 0);
        if (b.at(start) == player) {
            seen[start] = true;
            q.push(start);
        }
    }
    while (!q.empty()) {
        const int cur = q.front();
        q.pop();
        if (player == Color::red && b.row(cur) == n - 1) return true;
        if (player == Color::blue && b.col(cur) == n - 1) return true;
        for (int nb : b.neighbors(cur)) {
            if (!seen[nb] && b.at(nb) == player) {
                seen[nb] = true;
                q.push(nb);
            }
        }
    }
    return false;
}

}  // namespace

TEST_CASE("neighbor counts on a 5x5 board") {
    Board b(5);
    REQUIRE(b.neighbors(b.index(0, 0)).size() == 2);   // sharp corner
    REQUIRE(b.neighbors(b.index(4, 4)).size() == 2);   // sharp corner
    REQUIRE(b.neighbors(b.index(0, 4)).size() == 3);   // wide corner
    REQUIRE(b.neighbors(b.index(4, 0)).size() == 3);   // wide corner
    REQUIRE(b.neighbors(b.index(0, 2)).size() == 4);   // edge
    REQUIRE(b.neighbors(b.index(2, 2)).size() == 6);   // interior
}

TEST_CASE("new board is empty, red to move, no winner") {
    Board b(3);
    REQUIRE(b.to_move() == Color::red);
    REQUIRE(b.winner() == Color::empty);
    REQUIRE(b.legal_moves().size() == 9);
}

TEST_CASE("playing fills the cell and passes the turn") {
    Board b(3);
    b.play(b.index(1, 1));
    REQUIRE(b.at(b.index(1, 1)) == Color::red);
    REQUIRE(b.to_move() == Color::blue);
    REQUIRE_FALSE(b.is_legal(b.index(1, 1)));
    REQUIRE(b.legal_moves().size() == 8);
}

TEST_CASE("red wins with a full column, not with a gap") {
    Board b(3);
    // red plays column 0 top to bottom; blue plays harmless cells on column 2.
    b.play(b.index(0, 0));  // red
    b.play(b.index(0, 2));  // blue
    b.play(b.index(1, 0));  // red
    b.play(b.index(1, 2));  // blue
    REQUIRE(b.winner() == Color::empty);   // still a gap at (2, 0)
    b.play(b.index(2, 0));  // red closes the column
    REQUIRE(b.winner() == Color::red);
}

TEST_CASE("random full games: exactly one winner, matching BFS after every move") {
    std::mt19937 rng(12345);  // fixed seed: the same games every run
    for (int n = 1; n <= 11; ++n) {
        for (int game = 0; game < 50; ++game) {
            Board b(n);
            std::vector<int> moves = b.legal_moves();
            std::shuffle(moves.begin(), moves.end(), rng);
            for (int m : moves) {
                b.play(m);
                const bool red = bfs_connected(b, Color::red);
                const bool blue = bfs_connected(b, Color::blue);
                REQUIRE_FALSE((red && blue));        // never both
                if (b.winner() == Color::empty) {
                    REQUIRE_FALSE(red);
                    REQUIRE_FALSE(blue);
                } else {
                    REQUIRE(bfs_connected(b, b.winner()));
                }
            }
            REQUIRE(b.winner() != Color::empty);    // full board: no draws
        }
    }
}