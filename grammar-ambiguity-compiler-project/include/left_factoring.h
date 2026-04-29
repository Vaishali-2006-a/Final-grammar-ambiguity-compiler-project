#ifndef LEFT_FACTORING_H
#define LEFT_FACTORING_H

#include "grammar.h"
#include <string>

// ─────────────────────────────────────────────
// Left Factoring
// ─────────────────────────────────────────────
// Left factoring is required when two or more productions for the
// same NT share a common prefix, causing first-first conflicts in
// the LL(1) parsing table.
//
// Example:
//   A -> a b c | a b d | x
//
// Common prefix of first two: "a b"
// After left factoring:
//   A  -> a b A' | x
//   A' -> c | d
// ─────────────────────────────────────────────

class LeftFactoring {
public:
    // Perform left factoring on the entire grammar.
    // Returns a new (factored) Grammar.
    static Grammar factor(const Grammar& g);

    // Print a human-readable report of changes
    static void report(const Grammar& original, const Grammar& factored);

private:
    // Factor a single non-terminal (may iterate until stable)
    // Returns true if any change was made
    static bool factorNT(Grammar& g, const std::string& nt);

    // Find the longest common prefix among a subset of productions
    static std::vector<std::string> longestCommonPrefix(
        const std::vector<Production>& prods);

    // Generate a new NT name not already in the grammar
    static std::string newNTName(const Grammar& g, const std::string& base);
};

#endif // LEFT_FACTORING_H
