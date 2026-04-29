#include "parse_tree_generator.h"

#include "utils.h"

#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <utility>
#include <unordered_map>
#include <unordered_set>

namespace {

using StateId = int;

struct Piece {
    bool isTerminal = false;
    std::string symbol;
    std::string token;
    StateId childStateId = -1;
};

struct State {
    StateId id = -1;
    std::string lhs;
    Production rhs;
    std::size_t dot = 0;
    int start = 0;
    int end = 0;

    std::vector<std::vector<Piece>> derivations;
    std::unordered_set<std::string> derivationSignatures;

    bool isComplete() const {
        return dot >= rhs.size();
    }

    std::string nextSymbol() const {
        if (isComplete()) return "";
        return rhs[dot];
    }
};

struct ParseContext {
    std::vector<std::string> tokens;
    std::vector<State> states;
    std::vector<std::vector<StateId>> chart;
    std::vector<std::unordered_map<std::string, StateId>> chartIndex;

    std::string augmentedStart;
    std::string originalStart;
    std::set<std::string> nonTerminals;
};

std::string normalize_input(const std::string& input) {
    std::string trimmed = Utils::trim(input);
    if (trimmed == Utils::EPSILON) return "";
    return trimmed;
}

std::vector<std::string> parse_tokens(const std::string& input_string) {
    std::string normalized = normalize_input(input_string);
    if (normalized.empty()) return {};
    return Utils::tokenize(normalized);
}

Production normalize_production(const Production& p) {
    if (p.size() == 1 && p[0] == Utils::EPSILON) {
        return {};
    }
    return p;
}

std::string encode_piece(const Piece& piece) {
    if (piece.isTerminal) {
        return "T:" + piece.symbol + ":" + piece.token;
    }
    return "N:" + piece.symbol + ":" + std::to_string(piece.childStateId);
}

std::string encode_sequence(const std::vector<Piece>& seq) {
    std::ostringstream out;
    for (std::size_t i = 0; i < seq.size(); ++i) {
        if (i > 0) out << "|";
        out << encode_piece(seq[i]);
    }
    return out.str();
}

std::string encode_rhs(const Production& rhs) {
    std::ostringstream out;
    out << "[";
    for (std::size_t i = 0; i < rhs.size(); ++i) {
        if (i > 0) out << ",";
        out << rhs[i];
    }
    out << "]";
    return out.str();
}

std::string state_key(const std::string& lhs,
                      const Production& rhs,
                      std::size_t dot,
                      int start,
                      int end) {
    std::ostringstream out;
    out << lhs << "->" << encode_rhs(rhs)
        << "@" << dot << "(" << start << "," << end << ")";
    return out.str();
}

bool add_derivation(State& state, const std::vector<Piece>& sequence) {
    std::string sig = encode_sequence(sequence);
    if (state.derivationSignatures.insert(sig).second) {
        state.derivations.push_back(sequence);
        return true;
    }
    return false;
}

StateId add_state(ParseContext& ctx,
                  int chartPos,
                  const std::string& lhs,
                  const Production& rhs,
                  std::size_t dot,
                  int start,
                  int end,
                  const std::vector<std::vector<Piece>>& incomingDerivations) {
    std::string key = state_key(lhs, rhs, dot, start, end);
    auto& index = ctx.chartIndex[chartPos];
    auto it = index.find(key);

    if (it == index.end()) {
        State s;
        s.id = static_cast<StateId>(ctx.states.size());
        s.lhs = lhs;
        s.rhs = rhs;
        s.dot = dot;
        s.start = start;
        s.end = end;

        if (incomingDerivations.empty()) {
            add_derivation(s, {});
        } else {
            for (const auto& seq : incomingDerivations) {
                add_derivation(s, seq);
            }
        }

        ctx.states.push_back(s);
        StateId newId = s.id;
        index[key] = newId;
        ctx.chart[chartPos].push_back(newId);
        return newId;
    }

    StateId existingId = it->second;
    State& existing = ctx.states[existingId];
    for (const auto& seq : incomingDerivations) {
        add_derivation(existing, seq);
    }
    return existingId;
}

std::string unique_augmented_start(const Grammar& grammar) {
    std::string base = grammar.startSymbol + "_ROOT";
    std::string candidate = base;
    int suffix = 1;
    while (grammar.hasNonTerminal(candidate)) {
        candidate = base + std::to_string(suffix++);
    }
    return candidate;
}

ParseContext build_chart(const Grammar& grammar, const std::vector<std::string>& tokens) {
    ParseContext ctx;
    ctx.tokens = tokens;
    ctx.originalStart = grammar.startSymbol;
    ctx.augmentedStart = unique_augmented_start(grammar);

    ctx.nonTerminals.insert(grammar.nonTerminals.begin(), grammar.nonTerminals.end());
    ctx.nonTerminals.insert(ctx.augmentedStart);

    const int n = static_cast<int>(tokens.size());
    ctx.chart.resize(static_cast<std::size_t>(n + 1));
    ctx.chartIndex.resize(static_cast<std::size_t>(n + 1));

    add_state(ctx, 0, ctx.augmentedStart, Production{ctx.originalStart}, 0, 0, 0, {{}});

    for (int i = 0; i <= n; ++i) {
        for (std::size_t p = 0; p < ctx.chart[static_cast<std::size_t>(i)].size(); ++p) {
            StateId sid = ctx.chart[static_cast<std::size_t>(i)][p];
            State st = ctx.states[sid];

            if (!st.isComplete()) {
                std::string next = st.nextSymbol();
                bool nextIsNonTerminal = ctx.nonTerminals.count(next) > 0;

                if (nextIsNonTerminal) {
                    auto rulesIt = grammar.rules.find(next);
                    if (rulesIt != grammar.rules.end()) {
                        for (const auto& rawProd : rulesIt->second) {
                            Production prod = normalize_production(rawProd);
                            add_state(ctx, i, next, prod, 0, i, i, {{}});
                        }
                    }
                } else if (i < n && next == tokens[static_cast<std::size_t>(i)]) {
                    std::vector<std::vector<Piece>> advancedDerivations;
                    advancedDerivations.reserve(st.derivations.size());

                    for (const auto& seq : st.derivations) {
                        auto copy = seq;
                        Piece terminalPiece;
                        terminalPiece.isTerminal = true;
                        terminalPiece.symbol = next;
                        terminalPiece.token = tokens[static_cast<std::size_t>(i)];
                        copy.push_back(terminalPiece);
                        advancedDerivations.push_back(std::move(copy));
                    }

                    add_state(ctx,
                              i + 1,
                              st.lhs,
                              st.rhs,
                              st.dot + 1,
                              st.start,
                              i + 1,
                              advancedDerivations);
                }
            } else {
                int j = st.start;
                if (j < 0 || j > n) continue;

                const auto& originStates = ctx.chart[static_cast<std::size_t>(j)];
                for (StateId parentId : originStates) {
                    State parent = ctx.states[parentId];
                    if (parent.isComplete()) continue;
                    if (parent.nextSymbol() != st.lhs) continue;

                    std::vector<std::vector<Piece>> advancedDerivations;
                    advancedDerivations.reserve(parent.derivations.size());

                    for (const auto& pseq : parent.derivations) {
                        auto copy = pseq;
                        Piece ntPiece;
                        ntPiece.isTerminal = false;
                        ntPiece.symbol = st.lhs;
                        ntPiece.childStateId = sid;
                        copy.push_back(ntPiece);
                        advancedDerivations.push_back(std::move(copy));
                    }

                    add_state(ctx,
                              i,
                              parent.lhs,
                              parent.rhs,
                              parent.dot + 1,
                              parent.start,
                              i,
                              advancedDerivations);
                }
            }
        }
    }

    return ctx;
}

std::vector<StateId> find_final_states(const ParseContext& ctx) {
    std::vector<StateId> result;
    const int n = static_cast<int>(ctx.tokens.size());
    const auto& lastChart = ctx.chart[static_cast<std::size_t>(n)];

    for (StateId sid : lastChart) {
        const State& st = ctx.states[sid];
        if (st.lhs == ctx.augmentedStart &&
            st.isComplete() &&
            st.start == 0 &&
            st.end == n) {
            result.push_back(sid);
        }
    }

    return result;
}

std::string bracket_signature(const std::shared_ptr<TreeNode>& root) {
    if (!root) return "";
    return tree_to_bracket_text(root);
}

std::vector<std::shared_ptr<TreeNode>> cartesian_build(
    const std::string& lhs,
    const std::vector<std::vector<std::shared_ptr<TreeNode>>>& childOptions,
    std::size_t index,
    std::vector<std::shared_ptr<TreeNode>>& current,
    std::vector<std::shared_ptr<TreeNode>> acc) {

    if (index == childOptions.size()) {
        auto node = std::make_shared<TreeNode>(lhs);
        node->children = current;
        acc.push_back(node);
        return acc;
    }

    for (const auto& option : childOptions[index]) {
        current.push_back(option);
        acc = cartesian_build(lhs, childOptions, index + 1, current, std::move(acc));
        current.pop_back();
    }

    return acc;
}

std::vector<std::shared_ptr<TreeNode>> expand_state(
    StateId sid,
    const ParseContext& ctx,
    std::unordered_map<StateId, std::vector<std::shared_ptr<TreeNode>>>& memo,
    std::unordered_set<StateId>& inProgress) {

    auto itMemo = memo.find(sid);
    if (itMemo != memo.end()) return itMemo->second;

    if (inProgress.count(sid)) {
        return {};
    }

    inProgress.insert(sid);

    const State& st = ctx.states[sid];
    std::vector<std::shared_ptr<TreeNode>> trees;
    std::unordered_set<std::string> seen;

    for (const auto& seq : st.derivations) {
        if (st.rhs.empty()) {
            auto root = std::make_shared<TreeNode>(st.lhs);
            root->children.push_back(std::make_shared<TreeNode>(Utils::EPSILON));
            std::string sig = bracket_signature(root);
            if (seen.insert(sig).second) {
                trees.push_back(root);
            }
            continue;
        }

        if (seq.size() != st.rhs.size()) {
            continue;
        }

        std::vector<std::vector<std::shared_ptr<TreeNode>>> childOptions;
        bool valid = true;

        for (const auto& piece : seq) {
            if (piece.isTerminal) {
                auto leaf = std::make_shared<TreeNode>(piece.token);
                childOptions.push_back({leaf});
            } else {
                auto childTrees = expand_state(piece.childStateId, ctx, memo, inProgress);
                if (childTrees.empty()) {
                    valid = false;
                    break;
                }
                childOptions.push_back(std::move(childTrees));
            }
        }

        if (!valid) continue;

        std::vector<std::shared_ptr<TreeNode>> current;
        auto built = cartesian_build(st.lhs, childOptions, 0, current, {});
        for (auto& candidate : built) {
            std::string sig = bracket_signature(candidate);
            if (seen.insert(sig).second) {
                trees.push_back(std::move(candidate));
            }
        }
    }

    inProgress.erase(sid);
    memo[sid] = trees;
    return trees;
}

std::vector<std::shared_ptr<TreeNode>> strip_augmented_roots(
    const std::vector<std::shared_ptr<TreeNode>>& finalTrees) {
    std::vector<std::shared_ptr<TreeNode>> result;
    std::unordered_set<std::string> seen;

    for (const auto& t : finalTrees) {
        std::shared_ptr<TreeNode> candidate = t;
        if (t && t->children.size() == 1) {
            candidate = t->children[0];
        }

        std::string sig = bracket_signature(candidate);
        if (seen.insert(sig).second) {
            result.push_back(candidate);
        }
    }

    return result;
}

ParseTreesResult run_parser(const Grammar& grammar,
                            const std::string& input_string) {
    ParseTreesResult result;
    result.tokens = parse_tokens(input_string);

    if (grammar.startSymbol.empty()) {
        return result;
    }

    ParseContext ctx = build_chart(grammar, result.tokens);
    auto finals = find_final_states(ctx);
    if (finals.empty()) {
        return result;
    }

    result.accepted = true;

    std::unordered_map<StateId, std::vector<std::shared_ptr<TreeNode>>> memo;
    std::unordered_set<StateId> inProgress;
    std::vector<std::shared_ptr<TreeNode>> allFinalTrees;

    for (StateId fid : finals) {
        auto trees = expand_state(fid, ctx, memo, inProgress);
        allFinalTrees.insert(allFinalTrees.end(), trees.begin(), trees.end());
    }

    result.trees = strip_augmented_roots(allFinalTrees);
    result.treeCount = result.trees.size();
    return result;
}

} // namespace

ParseTreesResult generate_parse_trees(const Grammar& grammar,
                                      const std::string& input_string) {
    return run_parser(grammar, input_string);
}

std::size_t count_parse_trees(const Grammar& grammar,
                              const std::string& input_string) {
    return run_parser(grammar, input_string).treeCount;
}
