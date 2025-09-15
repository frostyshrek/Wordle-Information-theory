// wordle.h
#pragma once

#include <string>
#include <vector>
#include <set>

namespace wordle {

  // Renders the board + letters/bin
  void mapOut(std::vector<char>& letters, std::set<char>& bin, int tries);

  // Prints colored feedback for all guesses so far
  void checkGuess(const std::string& ans,
                  std::vector<char>& letters,
                  std::set<char>& bin,
                  const std::vector<std::string>& guesses);

  // Picks a random answer from file
  std::string ansGen();

  // autoplay entry point (bot plays a full game with no user input)
  void autoPlayLogic();

  // Runs one full game loop (may recurse for "play again")
  void mainLogic();

}
