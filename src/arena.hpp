#pragma once

#include <utility>

class Player;

struct MatchResults {
    int nr_games = 1000;
    int a_wins = 0;
    int a_games_as_red = 0;
    int a_wins_as_red = 0;
    int red_wins = 0;
    double game_run_time = 0;
};

int play_game(Player& red, Player& blue, int board_size, bool is_swap_on);

MatchResults run_match(Player& a, Player& b, int nr_games, int board_size, bool is_swap_on);

std::pair<double, double> wilson(int wins, int n, double z = 1.96);

void print_results(const MatchResults& results, const Player& a, const Player& b);