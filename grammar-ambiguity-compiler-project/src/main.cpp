// ═══════════════════════════════════════════════════════════════
//  Grammar Ambiguity Detector  –  main.cpp
//  Entry point: menu-driven CLI that ties all modules together.
// ═══════════════════════════════════════════════════════════════

#include "grammar.h"
#include "left_recursion.h"
#include "left_factoring.h"
#include "first_follow.h"
#include "ll1_table.h"
#include "ambiguity.h"
#include "parse_tree_integration.h"
#include "utils.h"

#include <iostream>
#include <string>
#include <limits>

using namespace Utils;

// ─────────────────────────────────────────────
// Application state (one active grammar)
// ─────────────────────────────────────────────
struct AppState {
    Grammar  grammar;
    FirstFollow ff;
    LL1Table    table;
    bool ffComputed    = false;
    bool tableBuilt    = false;
};

// ─────────────────────────────────────────────
// Print the main menu
// ─────────────────────────────────────────────
static void printMenu() {
    std::cout << "\n";
    printDivider('=');
    std::cout << "  GRAMMAR AMBIGUITY DETECTOR\n";
    printDivider('=');
    std::cout << "  1. Enter / Replace Grammar\n";
    std::cout << "  2. Display Current Grammar\n";
    std::cout << "  3. Detect Left Recursion\n";
    std::cout << "  4. Remove Left Recursion\n";
    std::cout << "  5. Perform Left Factoring\n";
    std::cout << "  6. Compute FIRST & FOLLOW Sets\n";
    std::cout << "  7. Generate LL(1) Parsing Table\n";
    std::cout << "  8. Check Ambiguity / Conflicts\n";
    std::cout << "  9. Full Analysis (all steps)\n";
    std::cout << " 10. Parse Trees for Input (Earley)\n";
    std::cout << "  0. Exit\n";
    printDivider('-');
    std::cout << "  Choose an option: ";
}

// ─────────────────────────────────────────────
// Ensure grammar is loaded; return false if not
// ─────────────────────────────────────────────
static bool requireGrammar(const AppState& s) {
    if (s.grammar.nonTerminals.empty()) {
        std::cout << "\n  [!] No grammar loaded. Please use option 1 first.\n";
        return false;
    }
    return true;
}

// ─────────────────────────────────────────────
// Run all analysis steps in sequence
// ─────────────────────────────────────────────
static void fullAnalysis(AppState& s) {
    if (!requireGrammar(s)) return;

    std::cout << "\n\n";
    printDivider('*', 60);
    std::cout << "  FULL GRAMMAR ANALYSIS\n";
    printDivider('*', 60);

    // 1. Display grammar
    std::cout << "\n  Current Grammar:\n";
    printDivider('-');
    s.grammar.print();
    printDivider('-');

    // 2. Left recursion detection
    std::cout << "\n";
    LeftRecursion::reportRecursion(s.grammar);

    // 3. Remove left recursion
    std::cout << "\n  Removing left recursion...\n";
    Grammar noLR = LeftRecursion::removeAll(s.grammar);
    std::cout << "\n  Grammar after removing left recursion:\n";
    printDivider('-');
    noLR.print();
    printDivider('-');

    // 4. Left factoring
    std::cout << "\n  Performing left factoring...\n";
    Grammar factored = LeftFactoring::factor(noLR);
    LeftFactoring::report(noLR, factored);

    // 5. FIRST / FOLLOW on factored grammar
    std::cout << "\n  Computing FIRST & FOLLOW sets...\n";
    s.grammar = factored;
    s.ff.compute(s.grammar);
    s.ffComputed = true;
    s.ff.printAll();

    // 6. LL(1) table
    std::cout << "\n  Building LL(1) table...\n";
    s.table.build(s.grammar, s.ff);
    s.tableBuilt = true;
    s.table.print(s.grammar);
    s.table.reportLL1Status();

    // 7. Ambiguity report
    Ambiguity::report(s.grammar, s.ff, s.table);

    printDivider('*', 60);
}

// ─────────────────────────────────────────────
// Run Earley parse-tree generation for one input
// ─────────────────────────────────────────────
static void runParseTreeAnalysis(const AppState& s) {
    if (!requireGrammar(s)) return;

    std::cout << "\nEnter input string (space-separated tokens, or 'e' for epsilon):\n";
    std::cout << "  input> ";
    std::string inputString;
    std::getline(std::cin, inputString);

    std::cout << "\nExport Graphviz DOT files? (y/n): ";
    std::string yn;
    std::getline(std::cin, yn);
    yn = Utils::trim(yn);

    ParseTreeIntegration::Options options;
    options.treesToPrint = 1;
    options.exportDot = (yn == "y" || yn == "Y");

    ParseTreeIntegration::runAndReport(s.grammar, inputString, options);
}

// ─────────────────────────────────────────────
// Main
// ─────────────────────────────────────────────
int main() {
    AppState state;
    std::string input;

    std::cout << "\n  Welcome to the Grammar Ambiguity Detector!\n";
    std::cout << "  Use 'e' to represent epsilon in productions.\n";

    while (true) {
        printMenu();

        if (!std::getline(std::cin, input)) break;
        input = Utils::trim(input);

        if (input == "0") {
            std::cout << "\n  Goodbye!\n\n";
            break;

        } else if (input == "1") {
            // ── Enter Grammar ─────────────────
            state.grammar.readFromUser();
            state.ffComputed  = false;
            state.tableBuilt  = false;

        } else if (input == "2") {
            // ── Display Grammar ───────────────
            std::cout << "\n";
            printDivider('=');
            std::cout << "  CURRENT GRAMMAR\n";
            printDivider('=');
            if (!requireGrammar(state)) continue;
            state.grammar.print();
            printDivider('-');

        } else if (input == "3") {
            // ── Detect Left Recursion ─────────
            if (!requireGrammar(state)) continue;
            LeftRecursion::reportRecursion(state.grammar);

        } else if (input == "4") {
            // ── Remove Left Recursion ─────────
            if (!requireGrammar(state)) continue;
            Grammar result = LeftRecursion::removeAll(state.grammar);
            std::cout << "\n";
            printDivider('=');
            std::cout << "  AFTER REMOVING LEFT RECURSION\n";
            printDivider('=');
            result.print();
            printDivider('-');

            std::cout << "\n  Apply this grammar? (y/n): ";
            std::string yn;
            std::getline(std::cin, yn);
            if (Utils::trim(yn) == "y" || Utils::trim(yn) == "Y") {
                state.grammar = result;
                state.ffComputed = false;
                state.tableBuilt = false;
                std::cout << "  Grammar updated.\n";
            }

        } else if (input == "5") {
            // ── Left Factoring ────────────────
            if (!requireGrammar(state)) continue;
            Grammar result = LeftFactoring::factor(state.grammar);
            LeftFactoring::report(state.grammar, result);

            std::cout << "\n  Apply this grammar? (y/n): ";
            std::string yn;
            std::getline(std::cin, yn);
            if (Utils::trim(yn) == "y" || Utils::trim(yn) == "Y") {
                state.grammar = result;
                state.ffComputed = false;
                state.tableBuilt = false;
                std::cout << "  Grammar updated.\n";
            }

        } else if (input == "6") {
            // ── FIRST & FOLLOW ────────────────
            if (!requireGrammar(state)) continue;
            state.ff.compute(state.grammar);
            state.ffComputed = true;
            state.tableBuilt = false;
            std::cout << "\n";
            state.ff.printAll();

        } else if (input == "7") {
            // ── LL(1) Table ───────────────────
            if (!requireGrammar(state)) continue;
            if (!state.ffComputed) {
                std::cout << "\n  [*] Computing FIRST & FOLLOW first...\n";
                state.ff.compute(state.grammar);
                state.ffComputed = true;
            }
            state.table.build(state.grammar, state.ff);
            state.tableBuilt = true;
            std::cout << "\n";
            state.table.print(state.grammar);
            state.table.reportLL1Status();

        } else if (input == "8") {
            // ── Ambiguity Check ───────────────
            if (!requireGrammar(state)) continue;
            if (!state.ffComputed) {
                std::cout << "\n  [*] Computing FIRST & FOLLOW first...\n";
                state.ff.compute(state.grammar);
                state.ffComputed = true;
            }
            if (!state.tableBuilt) {
                state.table.build(state.grammar, state.ff);
                state.tableBuilt = true;
            }
            Ambiguity::report(state.grammar, state.ff, state.table);

        } else if (input == "9") {
            // ── Full Analysis ─────────────────
            fullAnalysis(state);

        } else if (input == "10") {
            // ── Parse Trees (Earley) ──────────
            runParseTreeAnalysis(state);

        } else {
            std::cout << "\n  [!] Invalid option. Please enter 0–10.\n";
        }
    }

    return 0;
}
