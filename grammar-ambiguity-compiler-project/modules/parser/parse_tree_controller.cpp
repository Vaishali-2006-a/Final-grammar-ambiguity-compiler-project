#include "parse_tree_controller.h"

#include "parse_tree.h"
#include "utils.h"

#include <fstream>
#include <iostream>

ParseTreesResult ParseTreeController::run(const Grammar& grammar,
                                          const std::string& input_string) const {
    return generate_parse_trees(grammar, input_string);
}

ParseTreesResult ParseTreeController::runInteractive() {
    Grammar grammar;
    grammar.readFromUser();

    std::cout << "\nEnter input string (space-separated tokens, or 'e' for epsilon):\n";
    std::cout << "  input> ";

    std::string input;
    std::getline(std::cin, input);

    ParseTreesResult result = run(grammar, input);

    Utils::printDivider('=');
    std::cout << "PARSE TREE GENERATION RESULT\n";
    Utils::printDivider('=');

    if (!result.accepted) {
        std::cout << "Input rejected by grammar. No parse tree found.\n";
        return result;
    }

    std::cout << "Input accepted. Parse tree count: " << result.treeCount << "\n";
    if (result.isAmbiguous()) {
        std::cout << "Ambiguity detected: multiple parse trees exist.\n";
    }

    for (std::size_t i = 0; i < result.trees.size(); ++i) {
        std::cout << "\nTree #" << (i + 1) << " (indented):\n";
        std::cout << tree_to_indented_text(result.trees[i]);

        std::cout << "Tree #" << (i + 1) << " (bracket):\n";
        std::cout << tree_to_bracket_text(result.trees[i]) << "\n";
    }

    std::cout << "\nExport Graphviz DOT files? (y/n): ";
    std::string yn;
    std::getline(std::cin, yn);
    yn = Utils::trim(yn);

    if (yn == "y" || yn == "Y") {
        for (std::size_t i = 0; i < result.trees.size(); ++i) {
            std::string name = "parse_tree_" + std::to_string(i + 1) + ".dot";
            std::ofstream out(name);
            out << tree_to_dot(result.trees[i], "ParseTree" + std::to_string(i + 1));
            out.close();
            std::cout << "Wrote " << name << "\n";
        }
    }

    return result;
}
