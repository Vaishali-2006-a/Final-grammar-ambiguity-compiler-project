# Grammar Ambiguity Detector

A complete C++ project that analyzes Context-Free Grammars (CFGs) for
left recursion, ambiguity/conflicts, performs transformations, and
generates LL(1) parsing tables.

---

## Folder Structure

```
grammar_ambiguity_detector/
├── main.cpp              — Entry point, menu-driven CLI
├── grammar.h / .cpp      — Grammar data structures & I/O
├── utils.h / .cpp        — Shared utilities (trim, split, isTerminal, …)
├── left_recursion.h/.cpp — Detect & remove left recursion
├── left_factoring.h/.cpp — Perform left factoring
├── first_follow.h / .cpp — Compute FIRST and FOLLOW sets
├── ll1_table.h / .cpp    — Build & print the LL(1) parsing table
├── ambiguity.h / .cpp    — Detect First/First and First/Follow conflicts
├── Makefile              — Build script
└── README.md             — This file
```

---

## How to Compile

### Using Make (recommended)
```bash
make          # builds ./grammar_detector
make run      # builds and immediately runs
make clean    # removes object files and binary
```

### Using g++ directly
```bash
g++ -std=c++17 -Wall -O2 \
    main.cpp grammar.cpp utils.cpp \
    left_recursion.cpp left_factoring.cpp \
    first_follow.cpp ll1_table.cpp ambiguity.cpp \
    -o grammar_detector
```

Requires: g++ with C++17 support (GCC 7+ or Clang 5+).

---

## How to Run

```bash
./grammar_detector
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

## Sample Test Cases

### Test 1 — Classic Expression Grammar (Left Recursion)

**Input:**
```
E -> E + T | T
T -> T * F | F
F -> ( E ) | id
```

**What happens:**
- Detects immediate left recursion in E and T
- Removes it → produces E', T' non-terminals
- Computes correct FIRST/FOLLOW sets
- Generates valid LL(1) table
- Reports grammar IS LL(1)

---

### Test 2 — Ambiguous Dangling-Else Grammar

**Input:**
```
S -> i E t S S' | a
S' -> e S | e
E -> b
```

**What happens:**
- No left recursion
- Left factoring resolves S' conflict → introduces S''
- Final grammar is LL(1) after factoring

---

### Test 3 — Indirect Left Recursion

**Input:**
```
S -> A a | b
A -> A c | S d | e
```

**What happens:**
- Detects immediate LR in A
- Detects indirect LR in S (S → A → S)
- Removes both using the Dragon Book algorithm
- Reports resulting grammar conflicts if any remain

---

### Test 4 — Epsilon-Heavy Grammar (LL(1))

**Input:**
```
S -> a B D h
B -> c C
C -> b C | e
D -> E F
E -> g | e
F -> f | e
```

**What happens:**
- No recursion or conflicts
- FOLLOW sets properly propagated through nullable NTs
- LL(1) table cleanly populated

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

---

## Module Responsibilities

| Module            | Responsibility                                        |
|-------------------|-------------------------------------------------------|
| `utils`           | Trim, split, tokenize, isTerminal, setToString        |
| `grammar`         | Parsing input, storing rules, printing grammar        |
| `left_recursion`  | Detect (immediate + indirect) and remove LR           |
| `left_factoring`  | Detect common prefixes and factor grammar             |
| `first_follow`    | Compute FIRST and FOLLOW sets (fixed-point iteration) |
| `ll1_table`       | Build and print LL(1) parsing table, detect conflicts |
| `ambiguity`       | Report First/First and First/Follow conflicts         |
| `main`            | Menu-driven CLI, application state management         |
