#include <iostream>
#include <string>

#include "board.hpp"

namespace {
    const char* color_name(const Color& color) {
        if (color == Color::red) return "Red";
        else return "Blue";
    }

    char get_symbol(Color color) {
        switch(color) {
            case Color::red: return 'R';
            case Color::blue: return 'B';
            case Color::empty: return '.';
        }
    }

    void print(const Board& board) {
        std::cout << "  ";
        int n = board.size();
        for (int c = 0; c < n; c++) {
            std::cout << static_cast<char>('a' + c) << ' ';
        }
        std::cout << '\n';
        for (int r = 0; r < n; r++) {
            std::cout << std::string(r, ' ');
            std::cout << r + 1;
            if (r + 1 < 10) std::cout << ' ';
            for (int c = 0; c < n; c++) {
                std::cout << get_symbol(board.at(board.index(r, c))) << ' ';
            }
            std::cout << '\n';
        }
    }
}

int main(int argc, char* argv[]) {
    int n = (argc > 1 ? std::stoi(argv[1]) : 11);
    Board board(n);

    std::cout << "Red connect top-bottom, Blue connect left-right.\n";
    std::cout << "Enter moves like: a2. (q to quit)\n\n";

    while (board.winner() == Color::empty) {
        print(board);
        std::cout << color_name(board.to_move()) << " to move: ";

        std::string move;
        if (!(std::cin >> move) || move == "q") {
            return 0;
        }
        int c = move[0] - 'a', r = -1;
        try {
            r = std::stoi(move.substr(1)) - 1;
        } catch (...) {}

        if (move.size() < 2 || !board.inside_board(r, c) || !board.is_legal(board.index(r, c))) {
            std::cout << "Illegal move, try again!\n";
            continue;
        }
        board.play(board.index(r, c));
    }

    print(board);
    std::cout << color_name(board.winner()) << " wins!\n";
    return 0;
}