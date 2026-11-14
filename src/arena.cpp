#include "arena.hpp"
#include "board.hpp"

#include <iostream>
#include <chrono>
#include <cassert>
#include <cmath>
#include <format>
#include <string>

#include "players/player.hpp"

// 0 - red wins | 1 - blue wins
int play_game(Player &red, Player &blue, int board_size, bool is_swap_on) {
    Board board(board_size, is_swap_on);

    int turn = 0;
    while (board.winner() == Color::empty) {
        assert(!board.legal_moves().empty());
        int idx;
        idx = turn == 0 ? red.choose_move(board) : blue.choose_move(board);
        board.play(idx);
        turn ^= 1;
    }
    return board.winner() == Color::red ? 0 : 1;
}

MatchResults run_match(Player &a, Player &b, int nr_games, int board_size, bool is_swap_on) {
    MatchResults results;
    results.nr_games = nr_games;
    auto start_time = std::chrono::steady_clock::now();

    for (int game = 1; game <= nr_games; game++) {
        int winner = (game & 1) ? play_game(a, b, board_size, is_swap_on) : play_game(b, a, board_size, is_swap_on);
        results.a_wins += (game & 1) ? (winner == 0) : (winner == 1);
        results.a_games_as_red += game % 2;
        results.red_wins += 1 - winner;
        if (game & 1) results.a_wins_as_red += 1 - winner;
    }
    results.game_run_time = std::chrono::duration<double>(std::chrono::steady_clock::now() - start_time).count();
    return results;
}

std::pair<double, double> wilson(int wins, int n, double z) {
    if (n == 0) return {0.0, 1.0};
    double p_prim = (double) wins / n;
    double k = double(z * z) / n;
    // (p_prim - p)^2 <= k * p(1-p)
    // p_prim^2 - 2p_prim*p + p^2 <= kp - kp^2
    // p_prim^2 + (1+k)p^2 - (2p_prim+k)p <= 0
    // a = 1+k
    // b = -(2p_prim+k)
    // c = p_prim^2
    // delta = b^2 - 4ac
    // p = (-b +- sqrt(delta)) / 2a
    double a = k + 1.0;
    double b = -(2.0 * p_prim + k);
    double c = p_prim * p_prim;
    double delta = b * b - 4.0 * a * c;
    return {(-b - std::sqrt(delta)) / (2.0 * a), (-b + std::sqrt(delta)) / (2.0 * a)};
}

static void print_line(const std::string& s, int wins, int nr_games) {
    auto [lo, hi] = wilson(wins, nr_games);
    std::cout << std::format("{:<14} {:5.1f}% [{:5.1f}%, {:5.1f}%]\n", s, 100.0 * wins / nr_games, 100.0 * lo, 100.0 * hi);
}

void print_results(const MatchResults &results, const Player &a, const Player &b) {
    std::cout << a.player_type() << " vs " << b.player_type() << "  |  " << results.nr_games << " games ; ";
    std::cout << std::format("{:.1f} games/s\n\n", (double) results.nr_games / results.game_run_time);
    print_line("A wins", results.a_wins, results.nr_games);
    print_line("A wins as red", results.a_wins_as_red, results.a_games_as_red);
    print_line("A wins as blue", results.a_wins - results.a_wins_as_red, results.nr_games - results.a_games_as_red);
    print_line("Red wins", results.red_wins, results.nr_games);
}
