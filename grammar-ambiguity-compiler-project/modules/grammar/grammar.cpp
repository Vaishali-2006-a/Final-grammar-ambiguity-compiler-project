#include "grammar.h"
#include "utils.h"
#include <iostream>
#include <algorithm>

using namespace Utils;

// ─────────────────────────────────────────────
// Clear all grammar data
// ─────────────────────────────────────────────
void Grammar::clear() {
    rules.clear();
    nonTerminals.clear();
    startSymbol.clear();
}

// ─────────────────────────────────────────────
// Check whether a non-terminal has been registered
// ─────────────────────────────────────────────
bool Grammar::hasNonTerminal(const std::string& nt) const {
    return rules.find(nt) != rules.end();
}

// ─────────────────────────────────────────────
// Collect all terminals that appear in any production
// A symbol is a terminal if it's NOT a known non-terminal
// ─────────────────────────────────────────────
std::set<std::string> Grammar::getTerminals() const {
    std::set<std::string> terminals;
    for (const auto& pair : rules) {
        const auto& nt = pair.first;
        const auto& prods = pair.second;
        for (const auto& prod : prods) {
            for (const auto& sym : prod) {
                if (sym != EPSILON && !hasNonTerminal(sym)) {
                    terminals.insert(sym);
                }
            }
        }
    }
    return terminals;
}

// ─────────────────────────────────────────────
// Replace all productions for a given NT
// ─────────────────────────────────────────────
void Grammar::setProductions(const std::string& nt, const Productions& prods) {
    rules[nt] = prods;
}

// ─────────────────────────────────────────────
// Add a new NT (append to order list if not present)
// ─────────────────────────────────────────────
void Grammar::addNonTerminal(const std::string& nt, const Productions& prods) {
    if (!hasNonTerminal(nt)) {
        nonTerminals.push_back(nt);
    }
    rules[nt] = prods;
}

// ─────────────────────────────────────────────
// Validate if a rule follows CFG format
// ─────────────────────────────────────────────
bool Grammar::isValidCFGRule(const std::string& lhs, const std::string& rhs) const {
    // LHS must be a single non-terminal
    if (lhs.empty()) return false;
    
    // Check if LHS is a valid non-terminal (uppercase with optional primes)
    if (!Utils::isNonTerminal(lhs)) {
        return false;
    }
    
    // LHS should not contain spaces (must be single symbol)
    if (lhs.find(' ') != std::string::npos) {
        return false;
    }
    
    // RHS should not be empty (unless it's epsilon)
    std::string trimmedRhs = Utils::trim(rhs);
    if (trimmedRhs.empty()) {
        return false;
    }
    
    // Check each alternative in RHS
    auto alternatives = Utils::split(rhs, "|");
    for (const auto& alt : alternatives) {
        std::string trimmedAlt = Utils::trim(alt);
        if (trimmedAlt.empty()) {
            return false; // Empty alternative is invalid
        }
        
        // Check if this is epsilon (single 'e')
        if (trimmedAlt == "e") {
            continue; // Epsilon is valid
        }
        
        // Tokenize the alternative and check each symbol
        auto symbols = Utils::tokenize(trimmedAlt);
        for (const auto& sym : symbols) {
            // Each symbol must be either a terminal or non-terminal
            // No complex expressions or operators allowed in CFG
            if (sym == "->" || sym.find("->") != std::string::npos) {
                return false; // No nested productions
            }
        }
    }
    
    return true;
}

// ─────────────────────────────────────────────
// Parse a single line: "LHS -> alt1 | alt2 | ..."
// Each alternative is a space-separated list of symbols.
// Use 'e' to represent epsilon (empty production).
// ─────────────────────────────────────────────
void Grammar::parseRule(const std::string& line) {
    // Split on "->"
    auto arrowPos = line.find("->");
    if (arrowPos == std::string::npos) {
        std::cerr << "  [!] Invalid rule (missing '->'): " << line << "\n";
        return;
    }

    std::string lhs = trim(line.substr(0, arrowPos));
    std::string rhs = trim(line.substr(arrowPos + 2));

    if (lhs.empty()) {
        std::cerr << "  [!] Empty left-hand side.\n";
        return;
    }

    // Validate CFG format
    if (!isValidCFGRule(lhs, rhs)) {
        std::cerr << "  [!] Invalid CFG grammar rule. Context-Free Grammar must have:\n";
        std::cerr << "        - Single non-terminal on left-hand side (uppercase letter(s))\n";
        std::cerr << "        - Sequence of terminals and non-terminals on right-hand side\n";
        std::cerr << "        - No complex expressions or nested productions\n";
        std::cerr << "  [!] Rejected rule: " << line << "\n";
        return;
    }

    // Register NT in order list
    if (!hasNonTerminal(lhs)) {
        nonTerminals.push_back(lhs);
        if (startSymbol.empty()) startSymbol = lhs;
    }

    // Split alternatives on " | "
    auto alternatives = split(rhs, "|");
    Productions prods;
    for (auto& alt : alternatives) {
        alt = trim(alt);
        if (alt.empty()) continue;
        // Tokenize each alternative into symbols
        Production prod = tokenize(alt);
        if (!prod.empty()) prods.push_back(prod);
    }

    // Merge with any existing productions for this NT
    for (const auto& p : prods) {
        rules[lhs].push_back(p);
    }
}

// ─────────────────────────────────────────────
// Interactive grammar input from the user
// ─────────────────────────────────────────────
void Grammar::readFromUser() {
    clear();
    std::cout << "\n";
    printDivider('=');
    std::cout << "  ENTER CONTEXT-FREE GRAMMAR (CFG)\n";
    printDivider('=');
    std::cout << "  Format : LHS -> sym1 sym2 | sym3 sym4\n";
    std::cout << "  LHS must be a single non-terminal (uppercase letter(s))\n";
    std::cout << "  RHS must be terminals and non-terminals only\n";
    std::cout << "  Epsilon: use 'e' for empty production\n";
    std::cout << "  Example: E -> E + T | T\n";
    std::cout << "  Type 'done' on a blank line when finished.\n";
    printDivider('-');

    std::string line;
    while (true) {
        std::cout << "  rule> ";
        if (!std::getline(std::cin, line)) break;
        line = trim(line);
        if (line == "done" || line.empty()) {
            if (!nonTerminals.empty()) break;  // at least one rule required
            std::cout << "  [!] Please enter at least one production.\n";
            continue;
        }
        parseRule(line);
    }

    if (nonTerminals.empty()) {
        std::cout << "  [!] No grammar entered.\n";
        return;
    }

    std::cout << "\n  Grammar accepted. Start symbol: " << startSymbol << "\n";
    printDivider('-');
}

// ─────────────────────────────────────────────
// Display the grammar in a clean format
// ─────────────────────────────────────────────
void Grammar::print() const {
    if (nonTerminals.empty()) {
        std::cout << "  (no grammar loaded)\n";
        return;
    }
    for (const auto& nt : nonTerminals) {
        auto it = rules.find(nt);
        if (it == rules.end()) continue;
        std::cout << "  " << nt << " -> ";
        bool firstProd = true;
        for (const auto& prod : it->second) {
            if (!firstProd) std::cout << " | ";
            for (size_t i = 0; i < prod.size(); ++i) {
                if (i > 0) std::cout << " ";
                std::cout << prod[i];
            }
            firstProd = false;
        }
        std::cout << "\n";
    }
}
