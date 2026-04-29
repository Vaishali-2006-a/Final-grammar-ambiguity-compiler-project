#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H

#include "grammar.h"
#include <map>
#include <set>
#include <string>
#include <vector>

// ─────────────────────────────────────────────
// FIRST and FOLLOW set computation
// ─────────────────────────────────────────────

using FirstFollowMap = std::map<std::string, std::set<std::string>>;

class FirstFollow {
public:
    FirstFollowMap firstSets;
    FirstFollowMap followSets;

    // Compute both FIRST and FOLLOW sets for the given grammar
    void compute(const Grammar& g);

    // Print FIRST sets
    void printFirst() const;

    // Print FOLLOW sets
    void printFollow() const;

    // Print both
    void printAll() const;

    // FIRST of an arbitrary sequence of symbols (used in LL(1) table)
    // Returns FIRST(X1 X2 ... Xn)
    std::set<std::string> firstOfSequence(const Production& seq,
                                          const Grammar& g) const;

private:
    // Iteratively compute FIRST sets until stable
    void computeFirst(const Grammar& g);

    // Iteratively compute FOLLOW sets until stable
    void computeFollow(const Grammar& g);
};

#endif // FIRST_FOLLOW_H
