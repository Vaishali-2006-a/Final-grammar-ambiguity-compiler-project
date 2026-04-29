#include "parse_tree_controller.h"

#include <iostream>

int main() {
    std::cout << "\n============================================\n";
    std::cout << "  PARSE TREE GENERATOR (Earley-based)\n";
    std::cout << "============================================\n";

    ParseTreeController controller;
    controller.runInteractive();

    std::cout << "\nDone.\n";
    return 0;
}
