#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <set>
#include <map>

// ─────────────────────────────────────────────
// Utility helpers used across all modules
// ─────────────────────────────────────────────

namespace Utils {

// Trim leading/trailing whitespace from a string
std::string trim(const std::string& s);

// Split a string by a delimiter string (e.g., " | ")
std::vector<std::string> split(const std::string& s, const std::string& delim);

// Split a string by whitespace into tokens
std::vector<std::string> tokenize(const std::string& s);

// Check if a symbol is a terminal (lowercase, digit, special, or 'ε')
bool isTerminal(const std::string& sym);

// Check if a symbol is a non-terminal (uppercase letter(s))
bool isNonTerminal(const std::string& sym);

// Print a horizontal divider line
void printDivider(char ch = '-', int width = 60);

// Print a set of strings nicely: { a, b, c }
std::string setToString(const std::set<std::string>& s);

// The epsilon symbol used throughout the project
extern const std::string EPSILON;

// The end-of-input marker used in FOLLOW sets / LL(1) table
extern const std::string END_MARKER;

} // namespace Utils

#endif // UTILS_H
