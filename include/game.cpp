#include "game.hpp"

#include <ranges>
#include <string>

void Game::Reset() { board_.fill(0); }

Game::Game() { Reset(); }

bool Game::MakeMove(int player_ind, int pos) {
  if (pos < 1 || pos > 9 || board_[pos - 1] != 0) {
    return false;
  }
  board_[pos - 1] = player_ind;
  return true;
}

bool Game::CheckWin(int player_ind) const {
  for (const auto &pattern: win_patterns_) {
    if (board_[pattern[0]] == player_ind && board_[pattern[1]] == player_ind && board_[pattern[2]] == player_ind) {
      return true;
    }
  }
  return false;
}

bool Game::IsDraw() {
  return std::ranges::find(board_.begin(), board_.end(), 0) == board_.end();
}

std::string Game::BoardRepr() const {
  std::string result;
  for (int i = 0; i < 9; i++) {
    if (board_[i] == 0) {
      result += std::to_string(i + 1);
    } else {
      result += board_[i] == 1 ? "X" : "O";
    }
    if (i % 3 == 2 && i != 8) {
      result += "\n-+-+-\n";
    } else if (i % 3 != 2) {
      result += "|";
    }
  }
  return result;
}
