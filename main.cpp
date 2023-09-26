#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <set>
#include <cctype>
#include <fstream>


using namespace std;

void mapOut (vector<char>& letters, set<char>& bin, int tries){
    for(int i = 0; i<tries; i++){
        for(int j = 0; j<5; j++){
            cout << " _ ";
        };
        cout << '\n';
    };

    cout << '\n' << "Letters Available: ";
    for(int i = 0; i<letters.size(); i++){
        cout << letters[i] << " ";
    };
    cout << '\n' << "Bin: ";
    for (auto& i : bin) {
        cout << i << " ";
    }
    cout << '\n' << endl;
}

void checkGuess (string ans, vector<char>& letters, set<char>& bin,  vector<string>& guesses){
    cout << "\n";

    for(int j = 0; j<guesses.size(); j++){
        string temp = ans;
        for(int i = 0; i<5; i++){
            if(find(temp.begin(), temp.end(), guesses[j][i]) == temp.end()){
                letters.erase(remove(letters.begin(), letters.end(), guesses[j][i]), letters.end());
                bin.insert(guesses[j][i]);
                cout << " " << guesses[j][i] << " ";
            }else{
                if(guesses[j][i] == temp[i]){
                    temp[i] = '_';
                    cout << " " << "\033[32m" << guesses[j][i] << "\033[0m" << " ";
                }else{
                    temp.replace(temp.find(guesses[j][i]), 1, 1, '_');
                    cout << " " << "\033[33m" << guesses[j][i] << "\033[0m" << " ";
                }
            }
        }
        cout << "\n";
    }
}

string ansGen(){
    mt19937 rng(static_cast<unsigned int>(time(nullptr)));
    vector <string> pool;

    ifstream fin("./word-file/wordle-answers-alphabetical.txt");
    string i;
    while (fin >> i) {
        pool.push_back(i);
    }
    fin.close();

    uniform_int_distribution<size_t> dist(0, pool.size() - 1);
    size_t randomIndex = dist(rng);
    return pool[randomIndex];
};


void mainLogic(){
    cout << '\n';

    vector<string> validGuesses;

    ifstream fin("./word-file/valid-wordle-words.txt");
    string iti;
    while (fin >> iti) {
        validGuesses.push_back(iti);
    }
    fin.close();


    vector<char> letters {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    set<char> bin;
    vector<string> guesses;
    string ans = ansGen();
    bool win = false;

    
    int tries = 6;
    mapOut(letters, bin, tries);

    while(tries > 0){
        tries--;

        bool found = false;
        string guess;
        cout << "What is your guess? ";
        while(!found){
            cin >> guess;
            guess = guess.substr(0, 5);
            for(int i = 0; i<guess.size(); i++){
                guess[i] = tolower(guess[i]);
            }
            found = true;
            if(find(validGuesses.begin(), validGuesses.end(), guess) == validGuesses.end()){
                cout << '\n' << "\033[31m" << "Invalid guess. Please try again. \033[0m" << '\n';
                cout << "What is your guess? ";
                found = false;
            }
        }
        guesses.push_back(guess);
        if(guess == ans){
            win = true;
            checkGuess(guess, letters, bin, guesses);
            mapOut(letters, bin, tries);
            break;
        }

        cout << '\n';

        checkGuess(ans, letters, bin, guesses);
        mapOut(letters, bin, tries);
    };

    if(win){
        cout <<  "\033[32m" << "You Win!" << "\033[0m" << endl;
        cout << "The Correct Answer is: " << ans << '\n';

        string choice;
        cout << '\n' << "Play Again? (Y/N): ";
        cin >> choice;
        choice = choice.substr(0, 1);
        if(choice == "Y" || choice == "y"){
            cout << "Playing again..." << endl;
            mainLogic();
        }else if(choice == "N" || choice == "n"){
            cout << '\n' << "Thanks for playing!" << '\n' << endl;
            return;
        }
    }else{
        cout << "\033[31m" << "You lost!" << "\033[0m" << '\n';
        cout << "The Correct choice answer is: " << ans << '\n';

        string choice;
        cout << '\n' << "Play Again? (Y/N): ";
        cin >> choice;
        choice = choice.substr(0, 1);
        if(choice == "Y" || choice == "y"){
            cout << "Playing again..." << endl;
            mainLogic();
        }else if(choice == "N" || choice == "n"){
            cout << '\n' << "Thanks for playing!" << '\n' << endl;
            return;
        }
    }
}

int main()
{
    mainLogic();
    return 0;
}