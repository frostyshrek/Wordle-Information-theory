#include "bot.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <limits>

using std::string;
using std::vector;

namespace wordle {

// ---------- Helpers: feedback computation (duplicate-safe, two pass) ----------
// returns base-3 code in [0..242] where digit_i (0=grey,1=yellow,2=green) is for position i.
int WordleBot::feedbackCode(const string& answer, const string& guess) {
    // Assumes both length 5 lowercase a-z
    std::array<int, 26> counts{};
    std::array<int, 5> mark{}; // 2=green, 1=yellow, 0=grey; we will build an int code at the end

    // Pass 1: mark greens and count remaining answer letters
    for (int i = 0; i < 5; ++i) {
        if (guess[i] == answer[i]) {
            mark[i] = 2; // green
        } else {
            counts[answer[i] - 'a']++;
        }
    }
    // Pass 2: yellows/greys
    for (int i = 0; i < 5; ++i) {
        if (mark[i] == 2) continue;
        int idx = guess[i] - 'a';
        if (idx >= 0 && idx < 26 && counts[idx] > 0) {
            mark[i] = 1; // yellow
            counts[idx]--;
        } else {
            mark[i] = 0; // grey
        }
    }

    // Encode to base-3 integer (pos 0 is least significant digit)
    int code = 0;
    int pow3 = 1;
    for (int i = 0; i < 5; ++i) {
        code += mark[i] * pow3;
        pow3 *= 3;
    }
    return code;
}

std::string WordleBot::feedbackString(const string& answer, const string& guess) {
    // decode the base-3 code to "gyb" string for readability
    static const char map[3] = {'b', 'y', 'g'};
    string fb(5, 'b');
    // Re-run to get marks (so we don't have to decode int)
    std::array<int, 26> counts{};
    std::array<int, 5> mark{};

    for (int i = 0; i < 5; ++i) {
        if (guess[i] == answer[i]) {
            mark[i] = 2;
        } else {
            counts[answer[i] - 'a']++;
        }
    }
    for (int i = 0; i < 5; ++i) {
        if (mark[i] == 2) continue;
        int idx = guess[i] - 'a';
        if (idx >= 0 && idx < 26 && counts[idx] > 0) {
            mark[i] = 1;
            counts[idx]--;
        } else {
            mark[i] = 0;
        }
    }
    for (int i = 0; i < 5; ++i) fb[i] = map[mark[i]];
    return fb;
}

int WordleBot::encodeFeedback(const std::string& feedback) {
    // feedback is 5 chars over {'g','y','b'}
    int code = 0;
    int pow3 = 1;
    for (int i = 0; i < 5; ++i) {
        int d = 0;
        char c = feedback[i];
        if (c == 'y' || c == 'Y') d = 1;
        else if (c == 'g' || c == 'G') d = 2;
        // else 'b' => 0
        code += d * pow3;
        pow3 *= 3;
    }
    return code;
}

// ---------- Construction ----------
WordleBot::WordleBot(vector<string> allowed, vector<string> answers)
    : allowed_(std::move(allowed)),
      answersAll_(answers),    // keep a copy if you ever want to reset
      candidates_(std::move(answers)) {}

// ---------- Public API ----------
bool WordleBot::isValidGuess(const std::string& guess) const {
    if (allowed_.empty()) return true;
    return std::find(allowed_.begin(), allowed_.end(), guess) != allowed_.end();
}

void WordleBot::applyFeedback(const std::string& guess, const std::string& feedback) {
    filterWith(guess, encodeFeedback(feedback));
}

std::string WordleBot::suggest(const std::string& searchPool) const {
    if (candidates_.empty()) {
        // fallback: choose a common vowel-y word if lists got desynced
        return "raise";
    }
    if (candidates_.size() == 1) return candidates_.front();

    const vector<string>* poolPtr = nullptr;
    if (searchPool == "candidates" || allowed_.empty()) {
        poolPtr = &candidates_;
    } else {
        poolPtr = &allowed_;
    }
    const vector<string>& pool = *poolPtr;

    // Evaluate expected entropy for each word in the pool, pick the max
    const string* best = &pool.front();
    double bestH = -1.0;

    for (const auto& w : pool) {
        double h = expectedEntropyFor(w);

        // Tie-breaker: prefer actual candidates (so you can win immediately)
        if (h > bestH) {
            bestH = h;
            best = &w;
        } else if (std::abs(h - bestH) < 1e-9) {
            // optional: if tie, prefer a candidate
            bool bestIsCandidate = std::binary_search(candidates_.begin(), candidates_.end(), *best);
            bool wIsCandidate = std::binary_search(candidates_.begin(), candidates_.end(), w);
            if (wIsCandidate && !bestIsCandidate) best = &w;
        }
    }
    return *best;
}

// ---------- Core: expected entropy ----------
double WordleBot::expectedEntropyFor(const std::string& guess) const {
    // Partition remaining candidates by feedback pattern when 'guess' is played
    // There are at most 3^5 = 243 patterns.
    std::array<int, 243> buckets{};
    for (const auto& ans : candidates_) {
        int code = feedbackCode(ans, guess);
        buckets[code]++;
    }

    const double N = static_cast<double>(candidates_.size());
    double H = 0.0;
    for (int c : buckets) {
        if (c == 0) continue;
        double p = c / N;
        H -= p * std::log2(p);  // Shannon entropy
    }
    return H;
}

// ---------- Filter with constraints ----------
void WordleBot::filterWith(const std::string& guess, int encodedFeedback) {
    vector<string> next;
    next.reserve(candidates_.size());
    for (const auto& w : candidates_) {
        if (feedbackCode(w, guess) == encodedFeedback) {
            next.push_back(w);
        }
    }
    candidates_.swap(next);
}

}
