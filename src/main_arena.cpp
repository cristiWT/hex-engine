#include <iostream>

#include "arena.hpp"
#include "players/player.hpp"


#include <random>
#include <chrono>
#include <limits.h>
#include <functional>

struct Args {
    std::string a = "random", b = "random";
    int board_size = 11;
    int nr_games = 1000;
    uint64_t seed = std::random_device{}();
    bool is_swap_on = true;
};

void usage() {
    std::cout << "usage: arena [--a name] [--b name] [--size n] [--games n] [--seed s] [--no-swap]" << std::endl;
}

static Args parse_args(int argc, char** argv) {
    Args result;
    for (int i = 1; i < argc; i++) {
        std::string flag = argv[i];
        std::function<std::string()> next_arg = [&]() {
            if (i + 1 >= argc) {
                throw std::runtime_error("missing value for " + flag);
            }
            return argv[++i];
        };
        if (flag == "--a") result.a = next_arg();
        else if (flag == "--b") result.b = next_arg();
        else if (flag == "--size") result.board_size = std::stoi(next_arg());
        else if (flag == "--games") result.nr_games = std::stoi(next_arg());
        else if (flag == "--seed") result.seed = std::stoull(next_arg());
        else if (flag == "--no-swap") result.is_swap_on = false;
        else if (flag == "--help") { usage(); std::exit(0); }
        else throw std::runtime_error("unknown flag: " + flag);
    }
    if (result.board_size < 1) {
        throw std::runtime_error("board size must be positive");
    }
    if (result.nr_games < 1) {
        throw std::runtime_error("number of games must be positive");
    }
    return result;
}

std::unique_ptr<Player> make_player(const std::string& player_type, uint64_t seed) {
    if (player_type == "random") return make_random_player(seed);
    throw std::runtime_error("unknown player type: " + player_type);
}

int main(int argc, char** argv) {
    try {
        Args args = parse_args(argc, argv);
        auto a = make_player(args.a, args.seed);
        auto b = make_player(args.b, args.seed + 1);
        MatchResults results = run_match(*a, *b, args.nr_games, args.board_size, args.is_swap_on);
        print_results(results, *a, *b);
        std::cout << "on the seeds: " << args.seed << "; " << args.seed + 1 << '\n';
    } catch (std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        usage();
        return 1;
    }
}