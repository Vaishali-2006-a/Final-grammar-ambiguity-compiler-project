#include "utils.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cctype>

namespace Utils {

// Global constants
const std::string EPSILON    = "e";   // We use 'e' to represent ε
const std::string END_MARKER = "$";   // End-of-input marker

// ─────────────────────────────────────────────
// Trim leading and trailing whitespace
// ─────────────────────────────────────────────
std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// ─────────────────────────────────────────────
// Split string by a delimiter string
// e.g., "A | B | C" with delim " | " → {"A","B","C"}
// ─────────────────────────────────────────────
std::vector<std::string> split(const std::string& s, const std::string& delim) {
    std::vector<std::string> result;
    size_t pos = 0, found;
    while ((found = s.find(delim, pos)) != std::string::npos) {
        result.push_back(trim(s.substr(pos, found - pos)));
        pos = found + delim.size();
    }
    result.push_back(trim(s.substr(pos)));
    return result;
}

// ─────────────────────────────────────────────
// Tokenize by whitespace
// e.g., "E + T" → {"E","+","T"}
// ─────────────────────────────────────────────
std::vector<std::string> tokenize(const std::string& s) {
    std::vector<std::string> tokens;
    std::istringstream iss(s);
    std::string tok;
    while (iss >> tok) tokens.push_back(tok);
    return tokens;
}

// ─────────────────────────────────────────────
// A non-terminal is an uppercase letter (A-Z) optionally followed by
// one or more prime characters (').  Examples: E, T, E', T', S''.
// Everything else is a terminal: '+', '*', 'id', '(', ')', '$', 'e', etc.
// ─────────────────────────────────────────────
bool isTerminal(const std::string& sym) {
    if (sym.empty()) return false;
    // Must start with an uppercase letter to be a non-terminal
    if (!std::isupper(static_cast<unsigned char>(sym[0]))) return true;
    // Remaining characters must all be prime (') to qualify as NT
    for (size_t i = 1; i < sym.size(); ++i) {
        if (sym[i] != '\'') return true; // has non-prime after uppercase -> terminal
    }
    return false; // uppercase letter + optional primes -> non-terminal
}

bool isNonTerminal(const std::string& sym) {
    return !isTerminal(sym);
}

// ─────────────────────────────────────────────
// Print a divider line of given char and width
// ─────────────────────────────────────────────
void printDivider(char ch, int width) {
    std::cout << std::string(width, ch) << "\n";
}

// ─────────────────────────────────────────────
// Convert a set<string> to human-readable "{ a, b, c }"
// ─────────────────────────────────────────────
std::string setToString(const std::set<std::string>& s) {
    if (s.empty()) return "{ }";
    std::string result = "{ ";
    bool first = true;
    for (const auto& item : s) {
        if (!first) result += ", ";
        result += item;
        first = false;
    }
    result += " }";
    return result;
}

} // namespace Utils
