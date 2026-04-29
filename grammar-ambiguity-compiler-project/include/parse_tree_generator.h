#ifndef PARSE_TREE_GENERATOR_H
#define PARSE_TREE_GENERATOR_H

#include "grammar.h"
#include "parse_tree.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

struct ParseTreesResult {
    bool accepted = false;
    std::vector<std::string> tokens;
    std::vector<std::shared_ptr<TreeNode>> trees;
    std::size_t treeCount = 0;

    bool isAmbiguous() const { return treeCount > 1; }
};

// Main API: generates all parse trees for the input string under the given CFG.
ParseTreesResult generate_parse_trees(const Grammar& grammar,
                                      const std::string& input_string);

// Convenience API: count parse trees without printing logic in client code.
std::size_t count_parse_trees(const Grammar& grammar,
                              const std::string& input_string);

#endif // PARSE_TREE_GENERATOR_H
