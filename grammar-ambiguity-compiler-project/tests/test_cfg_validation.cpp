#include "grammar.h"
#include "utils.h"
#include <iostream>
#include <cassert>

int main() {
    Grammar g;
    
    std::cout << "Testing CFG validation...\n\n";
    
    // Test 1: Valid CFG rule
    std::cout << "Test 1: Valid CFG rule 'E -> E + T | T'\n";
    assert(g.isValidCFGRule("E", "E + T | T"));
    std::cout << "PASSED\n\n";
    
    // Test 2: Invalid LHS (lowercase)
    std::cout << "Test 2: Invalid LHS 'e -> T'\n";
    assert(!g.isValidCFGRule("e", "T"));
    std::cout << "PASSED\n\n";
    
    // Test 3: Invalid LHS (multiple symbols)
    std::cout << "Test 3: Invalid LHS 'E T -> T'\n";
    assert(!g.isValidCFGRule("E T", "T"));
    std::cout << "PASSED\n\n";
    
    // Test 4: Valid epsilon production
    std::cout << "Test 4: Valid epsilon 'S -> A | e'\n";
    assert(g.isValidCFGRule("S", "A | e"));
    std::cout << "PASSED\n\n";
    
    // Test 5: Invalid nested production
    std::cout << "Test 5: Invalid nested production 'A -> B -> C'\n";
    assert(!g.isValidCFGRule("A", "B -> C"));
    std::cout << "PASSED\n\n";
    
    // Test 6: Valid complex CFG
    std::cout << "Test 6: Valid complex CFG 'S -> ( S ) S | e'\n";
    assert(g.isValidCFGRule("S", "( S ) S | e"));
    std::cout << "PASSED\n\n";
    
    // Test 7: Invalid empty RHS
    std::cout << "Test 7: Invalid empty RHS 'A -> '\n";
    assert(!g.isValidCFGRule("A", ""));
    std::cout << "PASSED\n\n";
    
    // Test 8: Invalid empty alternative
    std::cout << "Test 8: Invalid empty alternative 'A -> B | | C'\n";
    assert(!g.isValidCFGRule("A", "B | | C"));
    std::cout << "PASSED\n\n";
    
    std::cout << "All CFG validation tests passed!\n";
    return 0;
}
