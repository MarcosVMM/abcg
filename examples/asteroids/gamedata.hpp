#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>
#include "user.hpp"

enum class Input { Right, Left, Down, Up, Fire, Pause };
enum class State { Playing, GameOver, Win,Menu, Pause};

struct GameData {
  State m_state{State::Menu};
  std::bitset<6> m_input;  // [fire, up, down, left, right]

};


#endif