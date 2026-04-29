# Grammar Ambiguity Detector

A complete C++ project that analyzes Context-Free Grammars (CFGs) for
left recursion, ambiguity/conflicts, performs transformations, and
generates LL(1) parsing tables.

---

## Folder Structure

```
grammar_ambiguity_detector/
├── src/                 — Main source files
│   ├── main.cpp         — Entry point, menu-driven CLI
│   └── utils.cpp        — Shared utilities
├── include/             — All header files
│   ├── grammar.h
│   ├── utils.h
│   ├── left_recursion.h
│   ├── left_factoring.h
│   ├── first_follow.h
│   ├── ll1_table.h
│   ├── ambiguity.h
│   ├── parse_tree.h
│   ├── parse_tree_generator.h
│   ├── parse_tree_controller.h
│   └── parse_tree_integration.h
├── modules/
│   ├── grammar/         — Grammar analysis modules
│   │   ├── grammar.cpp
│   │   ├── ambiguity.cpp
│   │   └── first_follow.cpp
│   ├── transform/       — Grammar transformation modules
│   │   ├── left_recursion.cpp
│   │   └── left_factoring.cpp
│   └── parser/          — Parsing and tree generation modules
│       ├── ll1_table.cpp
│       ├── parse_tree.cpp
│       ├── parse_tree_generator.cpp
│       ├── parse_tree_controller.cpp
│       ├── parse_tree_integration.cpp
│       └── parse_tree_runner.cpp
├── build/               — Object files and executable
├── tests/               — Test files and test cases
├── frontend/            — index.html (UI)
├── docs/                — README.md and documentation
├── Makefile             — Build script
└── README.md            — This file
```

---

## How to Compile

### Using Make (recommended)
```bash
make          # builds build/grammar_detector
make run      # builds and immediately runs
make clean    # removes object files and binary
```

---

## How to Run

```bash
./build/grammar_detector
```

You will see a numbered menu:

```
============================================================
  GRAMMAR AMBIGUITY DETECTOR
============================================================
  1. Enter / Replace Grammar
  2. Display Current Grammar
  3. Detect Left Recursion
  4. Remove Left Recursion
  5. Perform Left Factoring
  6. Compute FIRST & FOLLOW Sets
  7. Generate LL(1) Parsing Table
  8. Check Ambiguity / Conflicts
  9. Full Analysis (all steps)
  10. Parse Trees for Input (Earley)
  0. Exit
```

---

## Input Format

When you choose option 1, enter one production rule per line.

```
Format:  LHS -> sym1 sym2 | sym3 sym4
Epsilon: use 'e' to represent ε (empty production)
End:     type 'done' on a blank line
```

**Rules:**
- Non-terminals: uppercase letter(s) optionally followed by `'` — e.g., `E`, `T`, `E'`, `S''`
- Terminals: anything else — e.g., `+`, `*`, `id`, `(`, `)`, `a`, `b`
- Epsilon: the literal character `e`
- Alternatives separated by `|` (with spaces around it)
- Symbols separated by spaces

**Example session:**
```
  rule> E -> E + T | T
  rule> T -> T * F | F
  rule> F -> ( E ) | id
  rule> done
```

---

## Algorithm Explanations

### FIRST Sets
For each production `A -> X1 X2 ... Xn`:
1. Add `FIRST(X1) \ {ε}` to `FIRST(A)`
2. If `ε ∈ FIRST(X1)`, also add `FIRST(X2) \ {ε}`, and so on
3. If all Xi can derive ε, add ε to `FIRST(A)`

Iterate to fixed-point.

### FOLLOW Sets
1. Add `$` to `FOLLOW(start symbol)`
2. For each production `B -> α A β`:
   - Add `FIRST(β) \ {ε}` to `FOLLOW(A)`
   - If `ε ∈ FIRST(β)`: add `FOLLOW(B)` to `FOLLOW(A)`

Iterate to fixed-point.

### Left Recursion Removal (Dragon Book §4.3.3)
Order NTs as A1, A2, ..., An. For each Ai:
1. Substitute all Aj (j < i) into Ai's productions
2. Eliminate any immediate left recursion from Ai

### Left Factoring
For each NT, group productions by first symbol. If a group has a
common prefix α of length ≥ 1:
- Replace `A -> α β1 | α β2` with `A -> α A'`, `A' -> β1 | β2`
Repeat until no group has a common prefix.

### LL(1) Table Construction
For each production `A -> α`:
1. For each terminal `a ∈ FIRST(α) \ {ε}`: add `A -> α` to `M[A][a]`
2. If `ε ∈ FIRST(α)`: for each `b ∈ FOLLOW(A)`, add `A -> α` to `M[A][b]`

Any cell with > 1 entry means the grammar is NOT LL(1).
