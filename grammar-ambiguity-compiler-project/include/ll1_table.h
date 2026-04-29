#ifndef LL1_TABLE_H
#define LL1_TABLE_H

#include "grammar.h"
#include "first_follow.h"
#include <map>
#include <set>
#include <string>
#include <vector>
// Cell: list of productions that map to this (NT, terminal) entry
using TableCell = std::vector<Production>;

// Table: M[NT][terminal] = list of productions
using ParseTable = std::map<std::string, std::map<std::string, TableCell>>;

class LL1Table {
public:
    ParseTable table;
    bool isLL1 = true;  // false if any cell has multiple productions

    // Build the table given grammar + precomputed FIRST/FOLLOW
    void build(const Grammar& g, const FirstFollow& ff);

    // Print the table in a formatted grid
    void print(const Grammar& g) const;

    // Report whether the grammar is LL(1)
    void reportLL1Status() const;

private:
    // Collect all terminals used as table columns (including $)
    std::vector<std::string> getColumns(const Grammar& g) const;
};

#endif // LL1_TABLE_H
