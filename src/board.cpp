#include "board.hpp"

#include <cassert>
#include <array>

namespace {
    constexpr std::array<std::array<int, 2>, 6>directions_ = {{
        {{-1, 0}}, {{-1, 1}}, {{0, 1}}, {{1, 0}}, {{1, -1}}, {{0, -1}}
    }};
}

Color Board::opponent() const {
    switch (to_move_) {
        case Color::red: return Color::blue;
        case Color::blue: return Color::red;
        default: return Color::empty;
    }
}

Board::Board(int n)
    : n_(n),
    cell_(std::vector<Color>(n * n, Color::empty)),
    to_move_(Color::red),
    winner_(Color::empty),
    groups_(n * n + 4) {
    assert(n > 0);
    neighbors_.resize(n_ * n_);
    for (int i = 0; i < n_ * n_; i++) {
        const int r = row(i);
        const int c = col(i);
        for (auto [dr, dc]: directions_) {
            if (inside_board(r + dr, c + dc)) {
                neighbors_[i].push_back(index(r + dr, c + dc));
            }
        }
    }
}

int Board::size() const { return n_; }
int Board::index(int r, int c) const { return r * n_ + c; }
int Board::col(int idx) const { return idx % n_; }
int Board::row(int idx) const { return idx / n_; }

bool Board::inside_board(int r, int c) const {
    return 0 <= r && r < n_ && 0 <= c && c < n_;
}

Color Board::at(int idx) const { return cell_[idx]; }
Color Board::to_move() const { return to_move_; }
Color Board::winner() const { return winner_; }

bool Board::is_legal(int idx) const {
    return 0 <= idx && idx < n_ * n_ && cell_[idx] == Color::empty;
}

int Board::top_node() const { return n_ * n_; }
int Board::bottom_node() const { return n_ * n_ + 1; }
int Board::left_node() const { return n_ * n_ + 2; }
int Board::right_node() const { return n_ * n_ + 3; }

const std::vector<int>& Board::neighbors(int idx) const {
    return neighbors_[idx];
}

std::vector<int> Board::legal_moves() const {
    std::vector<int> answer;
    for (int i = 0; i < n_ * n_; i++) {
        if (cell_[i] == Color::empty) {
            answer.push_back(i);
        }
    }
    return answer;
}

void Board::play(int idx) {
    assert(is_legal(idx));

    Color me = to_move_;
    cell_[idx] = me;

    for (int neighbor: neighbors_[idx]) {
        if (cell_[neighbor] == me) {
            groups_.unite(idx, neighbor);
        }
    }
    const int r = row(idx);
    const int c = col(idx);
    if (me == Color::red) {
        if (r == 0) groups_.unite(top_node(), idx);
        if (r == n_ - 1) groups_.unite(bottom_node(), idx);
    } else {
        if (c == 0) groups_.unite(left_node(), idx);
        if (c == n_ - 1) groups_.unite(right_node(), idx);
    }

    if (winner_ == Color::empty) {
        if (groups_.same(top_node(), bottom_node())) winner_ = Color::red;
        if (groups_.same(left_node(), right_node())) winner_ = Color::blue;
    }
    to_move_ = Board::opponent();
}

