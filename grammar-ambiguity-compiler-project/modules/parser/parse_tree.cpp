#include "parse_tree.h"

#include <sstream>

TreeNode::TreeNode(const std::string& v) : value(v) {}

static void build_indented(const std::shared_ptr<TreeNode>& node,
                           std::ostringstream& out,
                           int depth) {
    if (!node) return;
    out << std::string(depth * 2, ' ') << node->value << "\n";
    for (const auto& child : node->children) {
        build_indented(child, out, depth + 1);
    }
}

std::string tree_to_indented_text(const std::shared_ptr<TreeNode>& root) {
    std::ostringstream out;
    build_indented(root, out, 0);
    return out.str();
}

static void build_bracket(const std::shared_ptr<TreeNode>& node,
                          std::ostringstream& out) {
    if (!node) return;
    out << "(" << node->value;
    for (const auto& child : node->children) {
        out << " ";
        build_bracket(child, out);
    }
    out << ")";
}

std::string tree_to_bracket_text(const std::shared_ptr<TreeNode>& root) {
    std::ostringstream out;
    build_bracket(root, out);
    return out.str();
}

static void build_dot(const std::shared_ptr<TreeNode>& node,
                      std::ostringstream& out,
                      int& nextId,
                      int parentId,
                      bool hasParent) {
    if (!node) return;

    int myId = nextId++;
    out << "  n" << myId << " [label=\"" << node->value << "\"];\n";
    if (hasParent) {
        out << "  n" << parentId << " -> n" << myId << ";\n";
    }

    for (const auto& child : node->children) {
        build_dot(child, out, nextId, myId, true);
    }
}

std::string tree_to_dot(const std::shared_ptr<TreeNode>& root,
                        const std::string& graphName) {
    std::ostringstream out;
    out << "digraph " << graphName << " {\n";
    out << "  rankdir=TB;\n";
    out << "  node [shape=box, style=rounded];\n";

    int nextId = 0;
    build_dot(root, out, nextId, -1, false);

    out << "}\n";
    return out.str();
}
