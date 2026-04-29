#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include <memory>
#include <string>
#include <vector>

class TreeNode {
public:
    std::string value;
    std::vector<std::shared_ptr<TreeNode>> children;

    explicit TreeNode(const std::string& v);
};

// Text output: indentation-based tree format.
std::string tree_to_indented_text(const std::shared_ptr<TreeNode>& root);

// Text output: bracketed format.
std::string tree_to_bracket_text(const std::shared_ptr<TreeNode>& root);

// Graphviz DOT output.
std::string tree_to_dot(const std::shared_ptr<TreeNode>& root,
                        const std::string& graphName = "ParseTree");

#endif // PARSE_TREE_H
