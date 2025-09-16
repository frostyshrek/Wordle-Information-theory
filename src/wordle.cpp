#include "wordle.h"
#include "bot.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>
#include <ctime>

static std::vector<std::string> loadList(const std::string& path) {
    std::ifstream fin(path);
    std::vector<std::string> v;
    std::string s;
    while (fin >> s) v.push_back(s);
    return v;
}

namespace wordle {

void mapOut(std::vector<char>& letters, std::set<char>& bin, int tries) {
    for (int i = 0; i < tries; i++) {
        for (int j = 0; j < 5; j++) std::cout << " _ ";
        std::cout << '\n';
    }
    std::cout << "\nLetters Available: ";
    for (char c : letters) std::cout << c << ' ';
    std::cout << "\nBin: ";
    for (auto& c : bin) std::cout << c << ' ';
    std::cout << "\n\n";
}

void checkGuess(const std::string& ans,
                std::vector<char>& letters,
                std::set<char>& bin,
                const std::vector<std::string>& guesses) {
    std::cout << '\n';
    for (const auto& g : guesses) {
        std::string temp = ans;
        for (int i = 0; i < 5; i++) {
            if (std::find(temp.begin(), temp.end(), g[i]) == temp.end()) {
                letters.erase(std::remove(letters.begin(), letters.end(), g[i]), letters.end());
                bin.insert(g[i]);
                std::cout << ' ' << g[i] << ' ';
            } else {
                if (g[i] == temp[i]) {
                    temp[i] = '_';
                    std::cout << " \033[32m" << g[i] << "\033[0m ";
                } else {
                    temp.replace(temp.find(g[i]), 1, 1, '_');
                    std::cout << " \033[33m" << g[i] << "\033[0m ";
                }
            }
        }
        std::cout << '\n';
    }
}

std::string ansGen(const std::vector<std::string>& answers) {
    if (answers.empty()) return "error";
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<size_t> dist(0, answers.size() - 1);
    return answers[dist(rng)];
}

void mainLogic() {
    std::cout << '\n';

    // adjust to your working directory:
    auto allowed = loadList("./data/valid-wordle-words.txt");
    auto answers = loadList("./data/wordle-answers-alphabetical.txt");
    if (allowed.empty() || answers.empty()) {
        std::cerr << "[ERROR] Could not load word lists.\n";
        return;
    }

    WordleBot bot(allowed, answers);

    std::vector<char> letters{
        'a','b','c','d','e','f','g','h','i','j','k','l','m',
        'n','o','p','q','r','s','t','u','v','w','x','y','z'
    };
    std::set<char> bin;
    std::vector<std::string> guesses;

    std::string ans = ansGen(answers);
    bool win = false;
    int tries = 6;

    mapOut(letters, bin, tries);

    while (tries > 0) {
        tries--;

        std::string hint = bot.suggest("candidates");
        std::cout << "[Hint] Try: " << hint << "\n";

        bool found = false;
        std::string guess;
        std::cout << "What is your guess? ";
        while (!found) {
            std::cin >> guess;
            if (guess.size() < 5) {
                std::cout << "\n\033[31mInvalid guess. Try again.\033[0m\n";
                std::cout << "What is your guess? ";
                continue;
            }
            guess = guess.substr(0, 5);
            for (char& c : guess) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));

            found = (std::find(allowed.begin(), allowed.end(), guess) != allowed.end());
            if (!found) {
                std::cout << "\n\033[31mInvalid guess. Try again.\033[0m\n";
                std::cout << "What is your guess? ";
            }
        }

        guesses.push_back(guess);

        std::string fb = WordleBot::feedbackString(ans, guess);
        bot.applyFeedback(guess, fb);

        if (guess == ans) {
            win = true;
            checkGuess(guess, letters, bin, guesses);
            mapOut(letters, bin, tries);
            break;
        }

        checkGuess(ans, letters, bin, guesses);
        mapOut(letters, bin, tries);
    }

    if (win) {
        std::cout << "\033[32mYou Win!\033[0m\n";
        std::cout << "The Correct Answer is: " << ans << '\n';
    } else {
        std::cout << "\033[31mYou lost!\033[0m\n";
        std::cout << "The Correct Answer is: " << ans << '\n';
    }

    std::string choice;
    std::cout << "\nPlay Again? (Y/N): ";
    std::cin >> choice;
    choice = choice.substr(0, 1);
    if (choice == "Y" || choice == "y") {
        std::cout << "Playing again...\n";
        mainLogic();
    } else {
        std::cout << "\nThanks for playing!\n\n";
    }
}

void autoPlayLogic() {
    std::cout << "\n[Auto-Play Mode]\n";

    auto allowed = loadList("./data/valid-wordle-words.txt");
    auto answers = loadList("./data/wordle-answers-alphabetical.txt");
    if (allowed.empty() || answers.empty()) {
        std::cerr << "[ERROR] Could not load word lists.\n";
        return;
    }

    WordleBot bot(allowed, answers);

    std::vector<char> letters{
        'a','b','c','d','e','f','g','h','i','j','k','l','m',
        'n','o','p','q','r','s','t','u','v','w','x','y','z'
    };
    std::set<char> bin;
    std::vector<std::string> guesses;

    std::string ans = ansGen(answers);
    bool win = false;
    int tries = 6;

    mapOut(letters, bin, tries);

    while (tries > 0) {
        tries--;

        std::string guess = bot.suggest("candidates");
        if (guess.empty()) {
            std::cerr << "[ERROR] No suggestion available.\n";
            return;
        }
        std::cout << "[Bot] guesses: " << guess << "\n";
        guesses.push_back(guess);

        std::string fb = WordleBot::feedbackString(ans, guess);
        bot.applyFeedback(guess, fb);

        checkGuess(ans, letters, bin, guesses);
        mapOut(letters, bin, tries);

        if (guess == ans) { win = true; break; }
    }

    if (win) {
        std::cout << "\033[32mBot wins!\033[0m\nAnswer: " << ans << '\n';
    } else {
        std::cout << "\033[31mBot failed in 6\033[0m\nAnswer was: " << ans << '\n';
    }

    std::string choice;
    std::cout << "\nPlay Again? (Y/N): ";
    std::cin >> choice;
    choice = choice.substr(0, 1);
    if (choice == "Y" || choice == "y") {
        std::cout << "Playing again...\n";
        autoPlayLogic();
    } else {
        std::cout << "\nThanks for playing!\n\n";
    }
}

}
