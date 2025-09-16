#pragma once
#include <string>
#include <vector>
#include <set>

namespace wordle {

// draw the empty rows + letters/bin
void mapOut(std::vector<char>& letters, std::set<char>& bin, int tries);

// print colored feedback for all guesses so far
void checkGuess(const std::string& ans,
                std::vector<char>& letters,
                std::set<char>& bin,
                const std::vector<std::string>& guesses);

// pick random answer from an already-loaded answers list
std::string ansGen(const std::vector<std::string>& answers);

// manual game loop
void mainLogic();

// auto-play (bot plays by itself)
void autoPlayLogic();

} // namespace wordle
