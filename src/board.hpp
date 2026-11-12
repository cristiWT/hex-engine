#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include "union_find.hpp"

enum class Color : std::uint8_t {empty, red, blue};

class Board {
public:
    explicit Board(int n);

    int size() const;
    int index(int r, int c) const;
    int col(int idx) const;
    int row(int idx) const;
    bool inside_board(int r, int c) const;

    Color at(int idx) const;
    Color to_move() const;
    Color winner() const;
    Color opponent() const;
    bool is_legal(int idx) const;
    bool is_swap_legal() const;

    void play(int idx);
    void perform_swap();

    std::vector<int> legal_moves() const;
    const std::vector<int>& neighbors(int idx) const;

private:
    int top_node() const;
    int bottom_node() const;
    int left_node() const;
    int right_node() const;

    int n_;
    std::vector<Color> cell_;
    Color to_move_;
    Color winner_;
    UnionFind groups_;
    std::vector<std::vector<int>> neighbors_;
    bool its_reds_first_move_, its_blues_first_move_;
};