#include "parse_tree_integration.h"

#include "parse_tree.h"
#include "utils.h"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace ParseTreeIntegration {

ParseTreesResult runAndReport(const Grammar& grammar,
                              const std::string& inputString,
                              const Options& options) {
    // Integration point requested: this call runs Earley-based parse tree generation.
    ParseTreesResult result = generate_parse_trees(grammar, inputString);

    Utils::printDivider('=');
    std::cout << "  EARLEY PARSE-TREE ANALYSIS\n";
    Utils::printDivider('=');

    if (!result.accepted) {
        std::cout << "Input accepted: No\n";
        std::cout << "Parse tree count: 0\n";
        std::cout << "Ambiguous for this input: No\n";
        return result;
    }

    std::cout << "Input accepted: Yes\n";
    std::cout << "Parse tree count: " << result.treeCount << "\n";
    std::cout << "Ambiguous for this input: "
              << (result.isAmbiguous() ? "Yes" : "No") << "\n";

    std::size_t treesToPrint = std::min(options.treesToPrint, result.trees.size());
    for (std::size_t i = 0; i < treesToPrint; ++i) {
        std::cout << "\nTree #" << (i + 1) << " (indented):\n";
        std::cout << tree_to_indented_text(result.trees[i]);

        std::cout << "Tree #" << (i + 1) << " (bracket):\n";
        std::cout << tree_to_bracket_text(result.trees[i]) << "\n";
    }

    if (options.exportDot) {
        for (std::size_t i = 0; i < result.trees.size(); ++i) {
            std::string fileName = options.dotFilePrefix + "_" + std::to_string(i + 1) + ".dot";
            std::ofstream out(fileName);
            out << tree_to_dot(result.trees[i], "ParseTree" + std::to_string(i + 1));
            std::cout << "Wrote " << fileName << "\n";
        }
    }

    return result;
}

} // namespace ParseTreeIntegration
