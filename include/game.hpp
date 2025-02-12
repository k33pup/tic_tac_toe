#pragma once

#include <array>

class Game {
 public:
  Game();

  bool MakeMove(int player_ind, int pos);

  [[nodiscard]] bool CheckWin(int player_ind) const;

  bool IsDraw();

  [[nodiscard]] std::string BoardRepr() const;

  void Reset();

 private:
  const std::array<std::array<int, 3>, 8> win_patterns_ = {
    {
      {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
      {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
      {0, 4, 8}, {2, 4, 6}
    }
  };
  std::array<int, 9> board_;
};
