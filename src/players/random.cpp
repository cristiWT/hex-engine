#include "players/player.hpp"
#include "board.hpp"

#include <random>
#include <vector>
#include <memory>

namespace {
    class RandomPlayer : public Player {
    public:
        explicit RandomPlayer(uint64_t seed): rng(seed) {}

        int choose_move(const Board& board) override {
            std::vector<int> moves = board.legal_moves();
            return moves[rng() % moves.size()];
        }

        std::string player_type() const override { return "random"; }
    private:
        std::mt19937_64 rng;
    };
}

std::unique_ptr<Player> make_random_player(uint64_t seed) {
    return std::make_unique<RandomPlayer>(seed);
}