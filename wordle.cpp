// wordle.cpp
#include "wordle.h"
#include "bot.h"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>

using namespace std;

namespace wordle {

static std::vector<std::string> loadList(const std::string& path) {
    std::ifstream fin(path);
    std::vector<std::string> v;
    std::string s;
    while (fin >> s) v.push_back(s);
    return v;
}

void autoPlayLogic() {
    cout << "\n[Auto-Play Mode]\n";

    // 1) Load lists and build the bot
    auto allowed = loadList("./word-file/valid-wordle-words.txt");
    auto answers = loadList("./word-file/wordle-answers-alphabetical.txt");
    wordle::WordleBot bot(allowed, answers);

    // 2) Game state (same visuals as manual play)
    vector<char> letters {
        'a','b','c','d','e','f','g','h','i','j','k','l','m',
        'n','o','p','q','r','s','t','u','v','w','x','y','z'
    };
    set<char> bin;
    vector<string> guesses;

    string ans = ansGen();       // random hidden answer
    bool win = false;
    int tries = 6;

    mapOut(letters, bin, tries);

    // 3) Let the bot play up to 6 turns
    while (tries > 0) {
        tries--;

        // Choose next guess (fast path: search only remaining candidates)
        string guess = bot.suggest("candidates");
        cout << "[Bot] guesses: " << guess << "\n";
        guesses.push_back(guess);

        // Compute feedback vs. the hidden answer and teach the bot
        string fb = wordle::WordleBot::feedbackString(ans, guess); // "gybbg"
        bot.applyFeedback(guess, fb);

        // Render board (all guesses so far)
        checkGuess(ans, letters, bin, guesses);
        mapOut(letters, bin, tries);

        if (guess == ans) {
            win = true;
            break;
        }
    }

    if (win) {
        cout << "\033[32m" << "Bot wins!" << "\033[0m" << endl;
        cout << "Answer: " << ans << '\n';
    } else {
        cout << "\033[31m" << "Bot failed in 6" << "\033[0m" << '\n';
        cout << "Answer was: " << ans << '\n';
    }

    // Replay prompt (auto-play again?)
    string choice;
    cout << '\n' << "Auto-play again? (Y/N): ";
    cin >> choice;
    choice = choice.substr(0, 1);
    if (choice == "Y" || choice == "y") {
        cout << "Running again...\n";
        autoPlayLogic();
    } else {
        cout << "\nThanks for watching the bot!\n\n";
    }
}

void mapOut(vector<char>& letters, set<char>& bin, int tries) {
    for (int i = 0; i < tries; i++) {
        for (int j = 0; j < 5; j++) {
            cout << " _ ";
        }
        cout << '\n';
    }

    cout << "\nLetters Available: ";
    for (char c : letters) cout << c << ' ';

    cout << "\nBin: ";
    for (auto& i : bin) cout << i << ' ';
    cout << "\n\n";
}

void checkGuess(const string& ans,
                vector<char>& letters,
                set<char>& bin,
                const vector<string>& guesses) {
    cout << "\n";
    for (const auto& g : guesses) {
        string temp = ans;
        for (int i = 0; i < 5; i++) {
            char ch = g[i];
            auto pos = temp.find(ch);
            if (pos == string::npos) {
                letters.erase(remove(letters.begin(), letters.end(), ch), letters.end());
                bin.insert(ch);
                cout << " " << ch << " ";
            } else {
                if (ch == temp[i]) {
                    temp[i] = '_';
                    cout << " " << "\033[32m" << ch << "\033[0m" << " ";
                } else {
                    temp[pos] = '_'; // safe since pos != npos
                    cout << " " << "\033[33m" << ch << "\033[0m" << " ";
                }
            }
        }
        cout << "\n";
    }
}

string ansGen() {
    mt19937 rng(static_cast<unsigned int>(time(nullptr)));
    vector<string> pool;

    ifstream fin("./word-file/wordle-answers-alphabetical.txt");
    string w;
    while (fin >> w) pool.push_back(w);
    fin.close();

    if (pool.empty()) return "error"; // simple guard

    uniform_int_distribution<size_t> dist(0, pool.size() - 1);
    return pool[dist(rng)];
}

void mainLogic() {
    std::cout << '\n';

    // Load word lists
    auto validGuesses = loadList("./word-file/valid-wordle-words.txt");
    auto answers      = loadList("./word-file/wordle-answers-alphabetical.txt");

    // Construct bot
    wordle::WordleBot bot(validGuesses, answers);

    // Game state
    std::vector<char> letters {
        'a','b','c','d','e','f','g','h','i','j','k','l','m',
        'n','o','p','q','r','s','t','u','v','w','x','y','z'
    };
    std::set<char> bin;
    std::vector<std::string> guesses;
    std::string ans = ansGen();
    bool win = false;

    int tries = 6;
    mapOut(letters, bin, tries);

    while (tries > 0) {
        tries--;

        // Bot suggests a word each turn
        std::string hint = bot.suggest("candidates");
        std::cout << "[Hint] Try: " << hint << "\n";

        // Ask player for a valid guess
        bool found = false;
        std::string guess;
        std::cout << "What is your guess? ";
        while (!found) {
            std::cin >> guess;
            if (guess.size() < 5) {
                std::cout << '\n' << "\033[31m"
                          << "Invalid guess. Please try again."
                          << "\033[0m" << '\n';
                std::cout << "What is your guess? ";
                continue;
            }
            guess = guess.substr(0, 5);
            for (char &c : guess)
                c = static_cast<char>(tolower(static_cast<unsigned char>(c)));

            found = (std::find(validGuesses.begin(),
                               validGuesses.end(),
                               guess) != validGuesses.end());

            if (!found) {
                std::cout << '\n' << "\033[31m"
                          << "Invalid guess. Please try again."
                          << "\033[0m" << '\n';
                std::cout << "What is your guess? ";
            }
        }

        guesses.push_back(guess);

        // Apply feedback to bot (so it learns)
        std::string fb = wordle::WordleBot::feedbackString(ans, guess);
        bot.applyFeedback(guess, fb);

        // Win check
        if (guess == ans) {
            win = true;
            checkGuess(guess, letters, bin, guesses);
            mapOut(letters, bin, tries);
            break;
        }

        std::cout << '\n';
        checkGuess(ans, letters, bin, guesses);
        mapOut(letters, bin, tries);
    }

    // End-of-game messages
    if (win) {
        std::cout << "\033[32m" << "You Win!" << "\033[0m" << std::endl;
        std::cout << "The Correct Answer is: " << ans << '\n';
    } else {
        std::cout << "\033[31m" << "You lost!" << "\033[0m" << '\n';
        std::cout << "The Correct Answer is: " << ans << '\n';
    }

    // Replay prompt
    std::string choice;
    std::cout << '\n' << "Play Again? (Y/N): ";
    std::cin >> choice;
    choice = choice.substr(0, 1);
    if (choice == "Y" || choice == "y") {
        std::cout << "Playing again..." << std::endl;
        mainLogic();
    } else {
        std::cout << '\n' << "Thanks for playing!" << '\n' << std::endl;
        return;
    }
}



}
