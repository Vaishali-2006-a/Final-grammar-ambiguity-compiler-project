#ifndef PARSE_TREE_INTEGRATION_H
#define PARSE_TREE_INTEGRATION_H

#include "grammar.h"
#include "parse_tree_generator.h"

#include <cstddef>
#include <string>

namespace ParseTreeIntegration {

struct Options {
    std::size_t treesToPrint = 1;
    bool exportDot = false;
    std::string dotFilePrefix = "parse_tree";
};

ParseTreesResult runAndReport(const Grammar& grammar,
                              const std::string& inputString,
                              const Options& options = {});

} // namespace ParseTreeIntegration

#endif // PARSE_TREE_INTEGRATION_H
