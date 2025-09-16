#include "wordle.h"
#include <iostream>

int main() {
    std::cout << "Select mode:\n"
              << "  1) Manual play\n"
              << "  2) Bot auto-play\n"
              << "Choice: ";
    int choice = 0;
    std::cin >> choice;

    if (choice == 2) wordle::autoPlayLogic();
    else             wordle::mainLogic();

    return 0;
}
