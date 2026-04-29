#include "left_recursion.h"
#include "utils.h"
#include <iostream>
#include <algorithm>
#include <functional>
#include <set>

using namespace Utils;

// ─────────────────────────────────────────────
// Detect Immediate Left Recursion
// A production A -> A … has immediate left recursion.
// ─────────────────────────────────────────────
std::vector<std::string> LeftRecursion::detectImmediate(const Grammar& g) {
    std::vector<std::string> result;
    for (const auto& nt : g.nonTerminals) {
        auto it = g.rules.find(nt);
        if (it == g.rules.end()) continue;
        for (const auto& prod : it->second) {
            if (!prod.empty() && prod[0] == nt) {
                result.push_back(nt);
                break; // one detection per NT is enough
            }
        }
    }
    return result;
}

// ─────────────────────────────────────────────
// Detect Indirect Left Recursion via DFS cycle detection
// For every NT, do a DFS following the first symbol of each production.
// If we reach the starting NT again, there is a cycle.
// ─────────────────────────────────────────────
std::vector<std::string> LeftRecursion::detectIndirect(const Grammar& g) {
    std::vector<std::string> cycleNTs;
    std::set<std::string> alreadyReported;

    // DFS helper: start from 'origin', current position 'cur'
    // 'visited' tracks the current DFS path to detect cycles
    std::function<bool(const std::string&, const std::string&,
                       std::set<std::string>&)> dfs;

    dfs = [&](const std::string& origin, const std::string& cur,
              std::set<std::string>& visited) -> bool {
        auto it = g.rules.find(cur);
        if (it == g.rules.end()) return false;

        for (const auto& prod : it->second) {
            if (prod.empty()) continue;
            const std::string& first = prod[0];

            if (first == origin) return true; // cycle found

            // Only recurse into non-terminals not yet on this path
            if (g.hasNonTerminal(first) && visited.find(first) == visited.end()) {
                visited.insert(first);
                if (dfs(origin, first, visited)) return true;
                visited.erase(first);
            }
        }
        return false;
    };

    for (const auto& nt : g.nonTerminals) {
        // Skip if immediate left recursion (handled separately)
        bool hasImmediate = false;
        auto it = g.rules.find(nt);
        if (it != g.rules.end()) {
            for (const auto& prod : it->second) {
                if (!prod.empty() && prod[0] == nt) { hasImmediate = true; break; }
            }
        }
        if (hasImmediate) continue;

        std::set<std::string> visited = {nt};
        if (dfs(nt, nt, visited)) {
            if (alreadyReported.find(nt) == alreadyReported.end()) {
                cycleNTs.push_back(nt);
                alreadyReported.insert(nt);
            }
        }
    }
    return cycleNTs;
}

// ─────────────────────────────────────────────
// Print a detection report
// ─────────────────────────────────────────────
void LeftRecursion::reportRecursion(const Grammar& g) {
    printDivider('=');
    std::cout << "  LEFT RECURSION DETECTION\n";
    printDivider('=');

    auto imm = detectImmediate(g);
    std::cout << "\n  [1] Immediate Left Recursion:\n";
    if (imm.empty()) {
        std::cout << "      None detected.\n";
    } else {
        for (const auto& nt : imm) {
            std::cout << "      " << nt << " has immediate left recursion\n";
        }
    }

    auto ind = detectIndirect(g);
    std::cout << "\n  [2] Indirect Left Recursion:\n";
    if (ind.empty()) {
        std::cout << "      None detected.\n";
    } else {
        for (const auto& nt : ind) {
            std::cout << "      " << nt << " participates in indirect left recursion\n";
        }
    }

    if (imm.empty() && ind.empty()) {
        std::cout << "\n  ✓ Grammar has NO left recursion.\n";
    } else {
        std::cout << "\n  ✗ Grammar has left recursion. Use option 3 to remove it.\n";
    }
    printDivider('-');
}

// ─────────────────────────────────────────────
// Generate a fresh non-terminal name
// Try "A'", "A''", "A1", "A2", ... until unique
// ─────────────────────────────────────────────
std::string LeftRecursion::newNTName(const Grammar& g, const std::string& base) {
    std::string candidate = base + "'";
    while (g.hasNonTerminal(candidate)) {
        candidate += "'";
    }
    return candidate;
}

// ─────────────────────────────────────────────
// Remove Immediate Left Recursion for a single NT
//
// Algorithm:
//   Partition productions of A into:
//     recursive:    A -> A α   (left-recursive)
//     nonrecursive: A -> β     (non-left-recursive)
//
//   Replace with:
//     A  -> β₁ A' | β₂ A' | ...
//     A' -> α₁ A' | α₂ A' | ... | e
//
//   (If no non-recursive alternatives exist the grammar is broken,
//    but we still handle it gracefully.)
// ─────────────────────────────────────────────
void LeftRecursion::removeImmediate(Grammar& g, const std::string& nt) {
    auto& prods = g.rules[nt];

    Productions recursive, nonRecursive;
    for (const auto& prod : prods) {
        if (!prod.empty() && prod[0] == nt) {
            // Strip the leading NT: A -> A α  ⟹  α
            recursive.push_back(Production(prod.begin() + 1, prod.end()));
        } else {
            nonRecursive.push_back(prod);
        }
    }

    if (recursive.empty()) return; // nothing to do

    // Create new NT  A'
    std::string newNT = newNTName(g, nt);

    // A  -> β A' for each β in nonRecursive
    Productions newProdsA;
    if (nonRecursive.empty()) {
        // Degenerate: only recursive prods — add e A'
        newProdsA.push_back({EPSILON, newNT});
    } else {
        for (const auto& beta : nonRecursive) {
            Production p = beta;
            if (p.size() == 1 && p[0] == EPSILON) {
                // β is epsilon: A -> e A' reduces to A -> A'
                p = {newNT};
            } else {
                p.push_back(newNT);
            }
            newProdsA.push_back(p);
        }
    }
    g.rules[nt] = newProdsA;

    // A' -> α A' for each α in recursive, plus A' -> e
    Productions newProdsAPrime;
    for (const auto& alpha : recursive) {
        Production p = alpha;
        if (p.empty()) {
            p = {newNT};
        } else {
            p.push_back(newNT);
        }
        newProdsAPrime.push_back(p);
    }
    newProdsAPrime.push_back({EPSILON}); // A' -> e

    // Register A' in grammar
    g.nonTerminals.push_back(newNT);
    g.rules[newNT] = newProdsAPrime;
}

// ─────────────────────────────────────────────
// Substitute productions of ntJ into the productions of ntI
// where ntI has a production starting with ntJ.
//
// For each production A -> B γ,  and each production B -> δ:
//   Add  A -> δ γ
// Remove the original A -> B γ
// ─────────────────────────────────────────────
void LeftRecursion::substituteProductions(Grammar& g,
                                          const std::string& ntI,
                                          const std::string& ntJ) {
    Productions& prodsI = g.rules[ntI];
    const Productions& prodsJ = g.rules[ntJ];

    Productions newProds;
    for (const auto& prodI : prodsI) {
        if (!prodI.empty() && prodI[0] == ntJ) {
            // Replace the leading ntJ with each production of ntJ
            Production suffix(prodI.begin() + 1, prodI.end());
            for (const auto& prodJ : prodsJ) {
                Production combined = prodJ;
                // Avoid double-epsilon: if prodJ is 'e' and suffix is empty → just 'e'
                if (combined.size() == 1 && combined[0] == EPSILON) {
                    if (suffix.empty()) {
                        newProds.push_back({EPSILON});
                    } else {
                        newProds.push_back(suffix);
                    }
                } else {
                    combined.insert(combined.end(), suffix.begin(), suffix.end());
                    newProds.push_back(combined);
                }
            }
        } else {
            newProds.push_back(prodI);
        }
    }
    prodsI = newProds;
}

// ─────────────────────────────────────────────
// Remove ALL Left Recursion (Algorithm from Dragon Book)
//
// 1. Order non-terminals as A1, A2, ..., An
// 2. For each Ai:
//    a. For each j < i:
//       - Replace Ai -> Aj γ with Ai -> δ1 γ | δ2 γ | ...
//         where Aj -> δ1 | δ2 | ...  (substitute)
//    b. Eliminate immediate left recursion from Ai
// ─────────────────────────────────────────────
Grammar LeftRecursion::removeAll(const Grammar& g) {
    Grammar ng = g; // work on a copy

    std::vector<std::string> order = ng.nonTerminals;

    for (size_t i = 0; i < order.size(); ++i) {
        const std::string& ntI = order[i];

        // Step a: substitute all Aj (j < i) into Ai
        for (size_t j = 0; j < i; ++j) {
            const std::string& ntJ = order[j];
            substituteProductions(ng, ntI, ntJ);
        }

        // Step b: remove immediate left recursion from Ai
        removeImmediate(ng, ntI);
    }

    return ng;
}
