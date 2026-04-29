#ifndef AMBIGUITY_H
#define AMBIGUITY_H

#include "grammar.h"
#include "first_follow.h"
#include "ll1_table.h"
#include <string>
#include <vector>


struct Conflict {
    std::string nonTerminal;
    std::string type;       // "First/First" or "First/Follow"
    std::string detail;     // human-readable description
};

class Ambiguity {
public:
    // Detect all conflicts and return them
    static std::vector<Conflict> detect(const Grammar& g, const FirstFollow& ff);

    // Print a formatted report
    static void report(const Grammar& g, const FirstFollow& ff,
                       const LL1Table& tbl);
};

#endif // AMBIGUITY_H
