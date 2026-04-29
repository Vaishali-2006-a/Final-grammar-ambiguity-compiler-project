#include "first_follow.h"
#include "utils.h"
#include <iostream>
#include <iomanip>

using namespace Utils;

void FirstFollow::computeFirst(const Grammar& g) {
    // Initialise: empty FIRST for every NT
    for (const auto& nt : g.nonTerminals) {
        firstSets[nt]; // default-construct empty set
    }

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& nt : g.nonTerminals) {
            auto it = g.rules.find(nt);
            if (it == g.rules.end()) continue;

            for (const auto& prod : it->second) {
                // prod is one alternative like {E, +, T} or {e}
                if (prod.size() == 1 && prod[0] == EPSILON) {
                    // A -> e: add e to FIRST(A)
                    if (!firstSets[nt].count(EPSILON)) {
                        firstSets[nt].insert(EPSILON);
                        changed = true;
                    }
                    continue;
                }

                // Walk through symbols in the production
                bool allDeriveEpsilon = true;
                for (const auto& sym : prod) {
                    if (sym == EPSILON) break;

                    std::set<std::string> symFirst;
                    if (Utils::isTerminal(sym)) {
                        symFirst.insert(sym); // FIRST of terminal is itself
                    } else {
                        symFirst = firstSets[sym]; // FIRST of NT
                    }

                    // Add FIRST(sym) \ {e} to FIRST(nt)
                    for (const auto& f : symFirst) {
                        if (f != EPSILON) {
                            if (!firstSets[nt].count(f)) {
                                firstSets[nt].insert(f);
                                changed = true;
                            }
                        }
                    }

                    if (!symFirst.count(EPSILON)) {
                        // e not in FIRST(sym), stop here
                        allDeriveEpsilon = false;
                        break;
                    }
                }

                if (allDeriveEpsilon) {
                    if (!firstSets[nt].count(EPSILON)) {
                        firstSets[nt].insert(EPSILON);
                        changed = true;
                    }
                }
            }
        }
    }
}

void FirstFollow::computeFollow(const Grammar& g) {
    // Initialise empty FOLLOW sets
    for (const auto& nt : g.nonTerminals) {
        followSets[nt];
    }

    // Rule 1: add $ to FOLLOW(start)
    followSets[g.startSymbol].insert(END_MARKER);

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& nt : g.nonTerminals) {
            auto it = g.rules.find(nt);
            if (it == g.rules.end()) continue;

            for (const auto& prod : it->second) {
                // Look at each symbol in the production
                for (size_t i = 0; i < prod.size(); ++i) {
                    const std::string& sym = prod[i];
                    if (!g.hasNonTerminal(sym)) continue; // only process NTs

                    // Compute FIRST of the suffix after sym
                    Production suffix(prod.begin() + i + 1, prod.end());
                    std::set<std::string> firstSuffix = firstOfSequence(suffix, g);

                    // Add FIRST(suffix) \ {e} to FOLLOW(sym)
                    for (const auto& f : firstSuffix) {
                        if (f != EPSILON) {
                            if (!followSets[sym].count(f)) {
                                followSets[sym].insert(f);
                                changed = true;
                            }
                        }
                    }

                    // If e ∈ FIRST(suffix) (or suffix is empty), add FOLLOW(nt)
                    if (firstSuffix.count(EPSILON) || suffix.empty()) {
                        for (const auto& f : followSets[nt]) {
                            if (!followSets[sym].count(f)) {
                                followSets[sym].insert(f);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

// ─────────────────────────────────────────────
// FIRST of a sequence of symbols  X1 X2 ... Xn
// ─────────────────────────────────────────────
std::set<std::string> FirstFollow::firstOfSequence(const Production& seq,
                                                    const Grammar& g) const {
    std::set<std::string> result;
    if (seq.empty()) {
        result.insert(EPSILON);
        return result;
    }

    bool allEpsilon = true;
    for (const auto& sym : seq) {
        if (sym == EPSILON) break;

        std::set<std::string> symFirst;
        if (Utils::isTerminal(sym)) {
            symFirst.insert(sym);
        } else {
            auto it = firstSets.find(sym);
            if (it != firstSets.end()) symFirst = it->second;
        }

        for (const auto& f : symFirst) {
            if (f != EPSILON) result.insert(f);
        }

        if (!symFirst.count(EPSILON)) {
            allEpsilon = false;
            break;
        }
    }

    if (allEpsilon) result.insert(EPSILON);
    return result;
}

// ─────────────────────────────────────────────
// Public entry point: compute both sets
// ─────────────────────────────────────────────
void FirstFollow::compute(const Grammar& g) {
    firstSets.clear();
    followSets.clear();
    computeFirst(g);
    computeFollow(g);
}

// ─────────────────────────────────────────────
// Pretty-print FIRST sets
// ─────────────────────────────────────────────
void FirstFollow::printFirst() const {
    printDivider('=');
    std::cout << "  FIRST SETS\n";
    printDivider('=');
    for (const auto& pair : firstSets) {
        const auto& nt = pair.first;
        const auto& fs = pair.second;
        std::cout << "  FIRST( " << std::setw(6) << std::left << nt << " ) = "
                  << setToString(fs) << "\n";
    }
    printDivider('-');
}

// ─────────────────────────────────────────────
// Pretty-print FOLLOW sets
// ─────────────────────────────────────────────
void FirstFollow::printFollow() const {
    printDivider('=');
    std::cout << "  FOLLOW SETS\n";
    printDivider('=');
    for (const auto& pair : followSets) {
        const auto& nt = pair.first;
        const auto& fs = pair.second;
        std::cout << "  FOLLOW( " << std::setw(6) << std::left << nt << " ) = "
                  << setToString(fs) << "\n";
    }
    printDivider('-');
}

// ─────────────────────────────────────────────
// Print both sets
// ─────────────────────────────────────────────
void FirstFollow::printAll() const {
    printFirst();
    std::cout << "\n";
    printFollow();
}
