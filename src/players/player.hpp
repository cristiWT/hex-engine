#pragma once

#include <string>
#include <memory>
#include <cstdint>

class Board;
class Player {
public:
    virtual ~Player() = default;
    virtual int choose_move(const Board& b) = 0;
    virtual std::string player_type() const = 0;
private:
};

std::unique_ptr<Player> make_random_player(uint64_t seed);
std::unique_ptr<Player> make_player(const std::string& player_type, uint64_t seed);