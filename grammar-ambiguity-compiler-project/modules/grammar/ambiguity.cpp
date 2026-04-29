#include "ambiguity.h"
#include "utils.h"
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace Utils;

// ─────────────────────────────────────────────
// Helper: intersection of two sets
// ─────────────────────────────────────────────
static std::set<std::string> intersect(const std::set<std::string>& a,
                                        const std::set<std::string>& b) {
    std::set<std::string> result;
    for (const auto& x : a) {
        if (b.count(x)) result.insert(x);
    }
    return result;
}

// ─────────────────────────────────────────────
// Helper: production to string
// ─────────────────────────────────────────────
static std::string prodStr(const Production& p) {
    std::string s;
    for (size_t i = 0; i < p.size(); ++i) {
        if (i > 0) s += " ";
        s += p[i];
    }
    return s;
}

// Detect First/First and First/Follow conflicts.
std::vector<Conflict> Ambiguity::detect(const Grammar& g, const FirstFollow& ff) {
    std::vector<Conflict> conflicts;

    for (const auto& nt : g.nonTerminals) {
        auto rIt = g.rules.find(nt);
        if (rIt == g.rules.end()) continue;
        const Productions& prods = rIt->second;

        // Precompute FIRST of each production
        std::vector<std::set<std::string>> firsts;
        for (const auto& prod : prods) {
            firsts.push_back(ff.firstOfSequence(prod, g));
        }

        // ── First/First check ────────────────
        for (size_t i = 0; i < prods.size(); ++i) {
            for (size_t j = i + 1; j < prods.size(); ++j) {
                auto inter = intersect(firsts[i], firsts[j]);
                inter.erase(EPSILON); // epsilon overlap handled by First/Follow
                if (!inter.empty()) {
                    Conflict c;
                    c.nonTerminal = nt;
                    c.type = "First/First";
                    std::ostringstream oss;
                    oss << nt << " -> " << prodStr(prods[i])
                        << "  AND  " << nt << " -> " << prodStr(prods[j])
                        << "  share FIRST symbols: " << setToString(inter);
                    c.detail = oss.str();
                    conflicts.push_back(c);
                }
            }
        }

        // ── First/Follow check ───────────────
        auto fwIt = ff.followSets.find(nt);
        const std::set<std::string>& followA =
            (fwIt != ff.followSets.end()) ? fwIt->second
                                          : *new std::set<std::string>();

        for (size_t i = 0; i < prods.size(); ++i) {
            if (!firsts[i].count(EPSILON)) continue; // ε not in FIRST(αi)

            // Check FIRST(αi) \ {ε} against FOLLOW(A)
            std::set<std::string> firstNoEps = firsts[i];
            firstNoEps.erase(EPSILON);
            auto inter = intersect(firstNoEps, followA);

            if (!inter.empty()) {
                Conflict c;
                c.nonTerminal = nt;
                c.type = "First/Follow";
                std::ostringstream oss;
                oss << nt << " -> " << prodStr(prods[i])
                    << "  (ε ∈ FIRST): overlapping symbols with FOLLOW("
                    << nt << "): " << setToString(inter);
                c.detail = oss.str();
                conflicts.push_back(c);
            }
        }
    }

    return conflicts;
}

// ─────────────────────────────────────────────
// Print a complete ambiguity/conflict report
// ─────────────────────────────────────────────
void Ambiguity::report(const Grammar& g, const FirstFollow& ff,
                       const LL1Table& tbl) {
    printDivider('=');
    std::cout << "  AMBIGUITY / CONFLICT ANALYSIS\n";
    printDivider('=');

    auto conflicts = detect(g, ff);

    if (conflicts.empty() && tbl.isLL1) {
        std::cout << "\n  ✓ No First/First or First/Follow conflicts detected.\n";
        std::cout << "  ✓ LL(1) table has no conflicts.\n";
        std::cout << "  ✓ Grammar appears to be unambiguous (for LL(1) purposes).\n";
    } else {
        std::cout << "\n  Conflicts found:\n\n";
        int i = 1;
        for (const auto& c : conflicts) {
            std::cout << "  [" << i++ << "] " << c.type << " conflict in <"
                      << c.nonTerminal << ">:\n";
            std::cout << "      " << c.detail << "\n\n";
        }

        if (!tbl.isLL1) {
            std::cout << "  Additionally, the LL(1) parsing table has cells\n";
            std::cout << "  with multiple productions (shown in the table above).\n\n";
        }

        std::cout << "  ✗ Grammar has conflicts/ambiguity.\n";
        std::cout << "  Tip: Apply Left Factoring (option 3) to resolve\n";
        std::cout << "       First/First conflicts.\n";
    }

    printDivider('-');
}
