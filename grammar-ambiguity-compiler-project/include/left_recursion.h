#ifndef LEFT_RECURSION_H
#define LEFT_RECURSION_H

#include "grammar.h"
#include <string>
#include <vector>

// ─────────────────────────────────────────────
// Left Recursion Detection & Removal
// ─────────────────────────────────────────────

class LeftRecursion {
public:
    // ── Detection ────────────────────────────

    // Return list of non-terminals that have IMMEDIATE left recursion
    // A -> A α  (first symbol of some production is A itself)
    static std::vector<std::string> detectImmediate(const Grammar& g);

    // Return list of (NT, path) pairs describing INDIRECT left recursion
    // A -> B α,  B -> A β  forms a cycle
    static std::vector<std::string> detectIndirect(const Grammar& g);

    // Print detection results
    static void reportRecursion(const Grammar& g);

    // ── Removal ──────────────────────────────

    // Remove ALL left recursion (immediate + indirect) using the standard
    // algorithm: order NTs, substitute, then eliminate immediate recursion.
    // Returns a new Grammar with left recursion removed.
    static Grammar removeAll(const Grammar& g);

private:
    // Remove immediate left recursion from a single non-terminal.
    // If A -> A α1 | A α2 | β1 | β2  then transform to:
    //   A  -> β1 A' | β2 A'
    //   A' -> α1 A' | α2 A' | e
    static void removeImmediate(Grammar& g, const std::string& nt);

    // Substitute productions of NTs[j] into NTs[i] where i > j
    // This eliminates indirect left recursion progressively
    static void substituteProductions(Grammar& g, const std::string& ntI,
                                      const std::string& ntJ);

    // Build a fresh non-terminal name that doesn't conflict (e.g., A')
    static std::string newNTName(const Grammar& g, const std::string& base);
};

#endif // LEFT_RECURSION_H
