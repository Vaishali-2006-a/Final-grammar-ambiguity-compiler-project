#ifndef PARSE_TREE_CONTROLLER_H
#define PARSE_TREE_CONTROLLER_H

#include "grammar.h"
#include "parse_tree_generator.h"

#include <string>

class ParseTreeController {
public:
    // Reads grammar from the same existing input format and parses one input string.
    ParseTreesResult runInteractive();

    // Non-interactive adapter function for integration with other runners/tools.
    ParseTreesResult run(const Grammar& grammar, const std::string& input_string) const;
};

#endif // PARSE_TREE_CONTROLLER_H
