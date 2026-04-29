#include "left_factoring.h"
#include "utils.h"
#include <iostream>
#include <algorithm>
#include <map>

using namespace Utils;

// ─────────────────────────────────────────────
// Generate a fresh NT name: base + "'" repeated until unique
// ─────────────────────────────────────────────
std::string LeftFactoring::newNTName(const Grammar& g, const std::string& base) {
    std::string candidate = base + "'";
    while (g.hasNonTerminal(candidate)) {
        candidate += "'";
    }
    return candidate;
}

// ─────────────────────────────────────────────
// Compute the longest common prefix of a list of productions.
// Uses pairwise comparison: start with prods[0] as prefix,
// then intersect with each subsequent production.
// ─────────────────────────────────────────────
std::vector<std::string> LeftFactoring::longestCommonPrefix(
    const std::vector<Production>& prods) {

    if (prods.empty()) return {};
    std::vector<std::string> prefix = prods[0];

    for (size_t i = 1; i < prods.size(); ++i) {
        const auto& p = prods[i];
        size_t len = std::min(prefix.size(), p.size());
        size_t k = 0;
        while (k < len && prefix[k] == p[k]) ++k;
        prefix.resize(k);
        if (prefix.empty()) break;
    }
    return prefix;
}

// ─────────────────────────────────────────────
// Factor one non-terminal.
// Algorithm:
//   Group productions by their first symbol.
//   For any group with ≥ 2 members that share a common prefix α:
//     Replace  A -> α β1 | α β2 | other_prods
//     With     A  -> α A' | other_prods
//              A' -> β1 | β2
//   Repeat until no more factoring is possible.
//
// Returns true if any productions were changed.
// ─────────────────────────────────────────────
bool LeftFactoring::factorNT(Grammar& g, const std::string& nt) {
    bool changed = false;

    // Keep looping until this NT is fully factored
    while (true) {
        Productions& prods = g.rules[nt];
        bool foundGroup = false;

        // Group productions by first symbol
        std::map<std::string, std::vector<size_t>> groups;
        for (size_t i = 0; i < prods.size(); ++i) {
            if (!prods[i].empty()) {
                groups[prods[i][0]].push_back(i);
            }
        }

        for (auto& pair : groups) {
            auto& firstSym = pair.first;
            auto& indices = pair.second;
            if (indices.size() < 2) continue; // no conflict

            // Collect the involved productions
            std::vector<Production> involved;
            for (size_t idx : indices) involved.push_back(prods[idx]);

            // Find longest common prefix
            std::vector<std::string> lcp = longestCommonPrefix(involved);
            if (lcp.empty()) continue;

            // We have a prefix to factor out
            foundGroup = true;
            changed = true;

            // Create new NT for the remainders
            std::string newNT = newNTName(g, nt);

            // Build productions for newNT: each remainder after stripping prefix
            Productions newNTProds;
            for (const auto& p : involved) {
                Production remainder(p.begin() + lcp.size(), p.end());
                if (remainder.empty()) {
                    newNTProds.push_back({EPSILON}); // A' -> e
                } else {
                    newNTProds.push_back(remainder);
                }
            }

            // Rebuild productions for nt:
            //   - Remove factored ones
            //   - Add A -> lcp newNT
            Productions newProdsNT;
            std::set<size_t> toRemove(indices.begin(), indices.end());
            for (size_t i = 0; i < prods.size(); ++i) {
                if (toRemove.find(i) == toRemove.end()) {
                    newProdsNT.push_back(prods[i]);
                }
            }
            Production factoredProd = lcp;
            factoredProd.push_back(newNT);
            newProdsNT.push_back(factoredProd);

            g.rules[nt] = newProdsNT;
            g.nonTerminals.push_back(newNT);
            g.rules[newNT] = newNTProds;

            break; // restart the while loop with updated prods
        }

        if (!foundGroup) break; // fully factored
    }

    return changed;
}

// ─────────────────────────────────────────────
// Apply left factoring to the entire grammar.
// We iterate over all original NTs (new ones added during factoring
// are also processed via the nonTerminals list which grows).
// ─────────────────────────────────────────────
Grammar LeftFactoring::factor(const Grammar& g) {
    Grammar ng = g;

    // We iterate with an index because new NTs may be appended
    for (size_t i = 0; i < ng.nonTerminals.size(); ++i) {
        factorNT(ng, ng.nonTerminals[i]);
    }

    return ng;
}

// ─────────────────────────────────────────────
// Print a before/after comparison
// ─────────────────────────────────────────────
void LeftFactoring::report(const Grammar& original, const Grammar& factored) {
    printDivider('=');
    std::cout << "  LEFT FACTORING\n";
    printDivider('=');

    std::cout << "\n  Original Grammar:\n";
    printDivider('-');
    original.print();

    std::cout << "\n  After Left Factoring:\n";
    printDivider('-');
    factored.print();
    printDivider('-');
}
