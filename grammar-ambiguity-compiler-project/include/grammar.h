#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include <vector>
#include <map>
#include <set>

// ─────────────────────────────────────────────
// Grammar data structures and I/O
// ─────────────────────────────────────────────

// A Production represents one alternative for a non-terminal.
// e.g., for E -> E + T | T
//   productions["E"] = { {"E","+","T"}, {"T"} }
using Production  = std::vector<std::string>;           // one RHS alternative
using Productions = std::vector<Production>;            // all alternatives for a NT

class Grammar {
public:
    // Map from non-terminal → list of productions
    std::map<std::string, Productions> rules;

    // Ordered list of non-terminals (preserves insertion order)
    std::vector<std::string> nonTerminals;

    // Start symbol (first non-terminal entered)
    std::string startSymbol;

    // ── I/O ──────────────────────────────────
    // Read grammar interactively from user
    void readFromUser();

    // Parse a single rule line like "E -> E + T | T"
    void parseRule(const std::string& line);

    // Print current grammar to stdout
    void print() const;

    // Clear all grammar data
    void clear();

    // Check if a non-terminal exists
    bool hasNonTerminal(const std::string& nt) const;

    // Get all terminal symbols that appear in the grammar
    std::set<std::string> getTerminals() const;

    // Replace the productions for a given NT
    void setProductions(const std::string& nt, const Productions& prods);

    // Append a new non-terminal with given productions
    void addNonTerminal(const std::string& nt, const Productions& prods);

    // Validate if a rule follows CFG format (single non-terminal LHS)
    bool isValidCFGRule(const std::string& lhs, const std::string& rhs) const;
};

#endif // GRAMMAR_H
