#include "ll1_table.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

using namespace Utils;

// ─────────────────────────────────────────────
// Collect all terminal columns for the table header.
// Includes all terminals from the grammar + "$".
// ─────────────────────────────────────────────
std::vector<std::string> LL1Table::getColumns(const Grammar& g) const {
    std::set<std::string> termSet = g.getTerminals();
    termSet.insert(END_MARKER);
    termSet.erase(EPSILON); // epsilon is not a column
    return std::vector<std::string>(termSet.begin(), termSet.end());
}

// ─────────────────────────────────────────────
// Build the LL(1) parsing table.
//
// For each production A -> α:
//   1. Compute FIRST(α)
//   2. For each terminal a in FIRST(α) (excluding ε):
//        Add A->α to M[A][a]
//   3. If ε ∈ FIRST(α):
//        For each terminal b in FOLLOW(A):
//          Add A->α to M[A][b]
//
// A grammar is LL(1) iff every cell has at most one production.
// ─────────────────────────────────────────────
void LL1Table::build(const Grammar& g, const FirstFollow& ff) {
    table.clear();
    isLL1 = true;

    for (const auto& nt : g.nonTerminals) {
        auto it = g.rules.find(nt);
        if (it == g.rules.end()) continue;

        for (const auto& prod : it->second) {
            // Compute FIRST of this production
            std::set<std::string> firstAlpha = ff.firstOfSequence(prod, g);

            // Rule 2: Add to M[nt][a] for each a in FIRST(α) \ {ε}
            for (const auto& a : firstAlpha) {
                if (a != EPSILON) {
                    table[nt][a].push_back(prod);
                    if (table[nt][a].size() > 1) isLL1 = false;
                }
            }

            // Rule 3: If ε ∈ FIRST(α), add to M[nt][b] for b in FOLLOW(nt)
            if (firstAlpha.count(EPSILON)) {
                auto fIt = ff.followSets.find(nt);
                if (fIt != ff.followSets.end()) {
                    for (const auto& b : fIt->second) {
                        table[nt][b].push_back(prod);
                        if (table[nt][b].size() > 1) isLL1 = false;
                    }
                }
            }
        }
    }
}

// ─────────────────────────────────────────────
// Helper: production vector → string "A B C"
// ─────────────────────────────────────────────
static std::string prodToStr(const std::string& nt, const Production& p) {
    std::string s = nt + " -> ";
    for (size_t i = 0; i < p.size(); ++i) {
        if (i > 0) s += " ";
        s += p[i];
    }
    return s;
}

// ─────────────────────────────────────────────
// Print the LL(1) parsing table in a grid format.
// Rows = non-terminals, Columns = terminals + "$"
// ─────────────────────────────────────────────
void LL1Table::print(const Grammar& g) const {
    printDivider('=');
    std::cout << "  LL(1) PARSING TABLE\n";
    printDivider('=');

    if (table.empty()) {
        std::cout << "  (table not built yet)\n";
        return;
    }

    std::vector<std::string> columns = getColumns(g);
    const int COL_W = 20; // column width for table cells
    const int NT_W  = 8;  // width for NT column

    // ── Header row ──────────────────────────
    std::cout << "  " << std::setw(NT_W) << std::left << "NT";
    for (const auto& col : columns) {
        std::cout << "| " << std::setw(COL_W - 2) << std::left << col;
    }
    std::cout << "\n";

    // ── Divider ──────────────────────────────
    std::cout << "  " << std::string(NT_W, '-');
    for (size_t i = 0; i < columns.size(); ++i) {
        std::cout << "+" << std::string(COL_W - 1, '-');
    }
    std::cout << "\n";

    // ── Data rows ────────────────────────────
    for (const auto& nt : g.nonTerminals) {
        std::cout << "  " << std::setw(NT_W) << std::left << nt;

        for (const auto& col : columns) {
            std::string cellStr;
            auto ntIt = table.find(nt);
            if (ntIt != table.end()) {
                auto colIt = ntIt->second.find(col);
                if (colIt != ntIt->second.end() && !colIt->second.empty()) {
                    // If multiple productions: show CONFLICT
                    if (colIt->second.size() > 1) {
                        cellStr = "[CONFLICT]";
                    } else {
                        // Show production RHS only (NT is the row)
                        const Production& p = colIt->second[0];
                        for (size_t k = 0; k < p.size(); ++k) {
                            if (k > 0) cellStr += " ";
                            cellStr += p[k];
                        }
                        // Prefix with "->"
                        cellStr = "-> " + cellStr;
                    }
                }
            }
            std::cout << "| " << std::setw(COL_W - 2) << std::left << cellStr;
        }
        std::cout << "\n";
    }

    printDivider('-');

    // ── Conflict details ─────────────────────
    bool anyConflict = false;
    for (const auto& nt : g.nonTerminals) {
        auto ntIt = table.find(nt);
        if (ntIt == table.end()) continue;
        for (const auto& pair : ntIt->second) {
            const auto& col = pair.first;
            const auto& prods = pair.second;
            if (prods.size() > 1) {
                if (!anyConflict) {
                    std::cout << "\n  CONFLICTS DETECTED:\n";
                    anyConflict = true;
                }
                std::cout << "  M[" << nt << ", " << col << "] has "
                          << prods.size() << " entries:\n";
                for (const auto& p : prods) {
                    std::cout << "    " << prodToStr(nt, p) << "\n";
                }
            }
        }
    }
    if (anyConflict) printDivider('-');
}

// ─────────────────────────────────────────────
// Report LL(1) status
// ─────────────────────────────────────────────
void LL1Table::reportLL1Status() const {
    printDivider('=');
    if (isLL1) {
        std::cout << "  ✓ The grammar IS LL(1).\n";
        std::cout << "  Each table cell has at most one production.\n";
    } else {
        std::cout << "  ✗ The grammar is NOT LL(1).\n";
        std::cout << "  One or more table cells have multiple productions (conflicts).\n";
        std::cout << "  Possible causes:\n";
        std::cout << "    - First/First conflict (two prods with same FIRST symbol)\n";
        std::cout << "    - First/Follow conflict (ε-production clashes with FOLLOW)\n";
    }
    printDivider('=');
}
