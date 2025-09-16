#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace wordle {

// Feedback encoding: 0 = grey/black, 1 = yellow, 2 = green
// Pattern is a base-3 number of length 5.

class WordleBot {
public:
    // allowed: all legal guesses
    // answers: possible solution list
    WordleBot(std::vector<std::string> allowed,
              std::vector<std::string> answers);

    // After each *real* guess, call this with the feedback pattern.
    // feedback: string of length 5 over {'g','y','b'}
    void applyFeedback(const std::string& guess, const std::string& feedback);

    // Suggest the next guess using expected-entropy over remaining candidates.
    // It evaluates entropy over candidates but can choose any allowed word.
    // If performance is a concern, pass searchPool="candidates" to restrict to candidates only.
    std::string suggest(const std::string& searchPool = "allowed") const;

    // Remaining candidates that could still be the answer
    const std::vector<std::string>& remaining() const { return candidates_; }

    // compute feedback for (answer, guess) in {'g','y','b'} string form
    static std::string feedbackString(const std::string& answer, const std::string& guess);

    // encode feedback to compact base-3 integer (0..242)
    static int encodeFeedback(const std::string& feedback);

    // Validate a guess is in the allowed list
    bool isValidGuess(const std::string& guess) const;

private:
    std::vector<std::string> allowed_;
    std::vector<std::string> answersAll_;   // original answers list for reference
    std::vector<std::string> candidates_;   // dynamic, shrinks as feedback is applied

    // compute entropy of trying 'guess' against current candidates_
    double expectedEntropyFor(const std::string& guess) const;

    // Filter candidates_ to only those that yield the given feedback when 'guess' is played
    void filterWith(const std::string& guess, int encodedFeedback);

    // Feedback internals (two-pass duplicate-safe)
    static int feedbackCode(const std::string& answer, const std::string& guess);
};

}
