# ═══════════════════════════════════════════════════════════════════════════
#  Grammar Ambiguity Detector  –  Makefile (Modular Build System)
# ═══════════════════════════════════════════════════════════════════════════

# ─────────────────────────────────────────────────────────────────────────
# COMPILER AND FLAGS
# ─────────────────────────────────────────────────────────────────────────
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
INCLUDE  := -I include

# ─────────────────────────────────────────────────────────────────────────
# DIRECTORIES
# ─────────────────────────────────────────────────────────────────────────
SRC_DIR         := src
MODULES_DIR     := modules
INCLUDE_DIR     := include
BUILD_DIR       := build
TESTS_DIR       := tests

# Module subdirectories
GRAMMAR_DIR     := $(MODULES_DIR)/grammar
PARSER_DIR      := $(MODULES_DIR)/parser
TRANSFORM_DIR   := $(MODULES_DIR)/transform

# Object file directories
BUILD_SRC       := $(BUILD_DIR)/src
BUILD_GRAMMAR   := $(BUILD_DIR)/modules/grammar
BUILD_PARSER    := $(BUILD_DIR)/modules/parser
BUILD_TRANSFORM := $(BUILD_DIR)/modules/transform

# ─────────────────────────────────────────────────────────────────────────
# SOURCE FILES (organized by module)
# ─────────────────────────────────────────────────────────────────────────

# Core utilities and main
SRC_FILES := \
	$(SRC_DIR)/utils.cpp \
	$(SRC_DIR)/main.cpp

# Grammar analysis module
GRAMMAR_FILES := \
	$(GRAMMAR_DIR)/grammar.cpp \
	$(GRAMMAR_DIR)/ambiguity.cpp \
	$(GRAMMAR_DIR)/first_follow.cpp

# Parse tree module
PARSER_FILES := \
	$(PARSER_DIR)/ll1_table.cpp \
	$(PARSER_DIR)/parse_tree.cpp \
	$(PARSER_DIR)/parse_tree_generator.cpp \
	$(PARSER_DIR)/parse_tree_controller.cpp \
	$(PARSER_DIR)/parse_tree_integration.cpp

# Grammar transformation module
TRANSFORM_FILES := \
	$(TRANSFORM_DIR)/left_recursion.cpp \
	$(TRANSFORM_DIR)/left_factoring.cpp

# All source files
ALL_SRC := $(SRC_FILES) $(GRAMMAR_FILES) $(PARSER_FILES) $(TRANSFORM_FILES)

# ─────────────────────────────────────────────────────────────────────────
# OBJECT FILES (mirrored directory structure)
# ─────────────────────────────────────────────────────────────────────────
SRC_OBJS      := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(BUILD_SRC)/%.o)
GRAMMAR_OBJS  := $(GRAMMAR_FILES:$(GRAMMAR_DIR)/%.cpp=$(BUILD_GRAMMAR)/%.o)
PARSER_OBJS   := $(PARSER_FILES:$(PARSER_DIR)/%.cpp=$(BUILD_PARSER)/%.o)
TRANSFORM_OBJS := $(TRANSFORM_FILES:$(TRANSFORM_DIR)/%.cpp=$(BUILD_TRANSFORM)/%.o)

ALL_OBJS := $(SRC_OBJS) $(GRAMMAR_OBJS) $(PARSER_OBJS) $(TRANSFORM_OBJS)

# ─────────────────────────────────────────────────────────────────────────
# EXECUTABLE
# ─────────────────────────────────────────────────────────────────────────
EXECUTABLE := $(BUILD_DIR)/grammar_detector

# ─────────────────────────────────────────────────────────────────────────
# DEFAULT TARGET
# ─────────────────────────────────────────────────────────────────────────
.PHONY: all
all: $(EXECUTABLE)

# ─────────────────────────────────────────────────────────────────────────
# LINK TARGET: Create executable from object files
# ─────────────────────────────────────────────────────────────────────────
$(EXECUTABLE): $(ALL_OBJS) | $(BUILD_DIR)
	@echo "Linking $(EXECUTABLE)..."
	$(CXX) $(CXXFLAGS) $(ALL_OBJS) -o $(EXECUTABLE)
	@echo "✓ Build complete: $(EXECUTABLE)"

# ─────────────────────────────────────────────────────────────────────────
# COMPILE TARGETS: Core (src/)
# ─────────────────────────────────────────────────────────────────────────
$(BUILD_SRC)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_SRC)
	@echo "  [SRC] Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# ─────────────────────────────────────────────────────────────────────────
# COMPILE TARGETS: Grammar module (modules/grammar/)
# ─────────────────────────────────────────────────────────────────────────
$(BUILD_GRAMMAR)/%.o: $(GRAMMAR_DIR)/%.cpp | $(BUILD_GRAMMAR)
	@echo "  [GRAMMAR] Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# ─────────────────────────────────────────────────────────────────────────
# COMPILE TARGETS: Parser module (modules/parser/)
# ─────────────────────────────────────────────────────────────────────────
$(BUILD_PARSER)/%.o: $(PARSER_DIR)/%.cpp | $(BUILD_PARSER)
	@echo "  [PARSER] Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# ─────────────────────────────────────────────────────────────────────────
# COMPILE TARGETS: Transform module (modules/transform/)
# ─────────────────────────────────────────────────────────────────────────
$(BUILD_TRANSFORM)/%.o: $(TRANSFORM_DIR)/%.cpp | $(BUILD_TRANSFORM)
	@echo "  [TRANSFORM] Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# ─────────────────────────────────────────────────────────────────────────
# DIRECTORY CREATION
# ─────────────────────────────────────────────────────────────────────────
$(BUILD_DIR) $(BUILD_SRC) $(BUILD_GRAMMAR) $(BUILD_PARSER) $(BUILD_TRANSFORM):
	@if not exist $@ (mkdir "$@")

# ─────────────────────────────────────────────────────────────────────────
# UTILITY TARGETS
# ─────────────────────────────────────────────────────────────────────────

# Run the compiled executable
.PHONY: run
run: $(EXECUTABLE)
	@echo "Running $(EXECUTABLE)..."
	./$(EXECUTABLE)

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@echo "✓ Clean complete"

# Show variables
.PHONY: info
info:
	@echo "── Grammar Ambiguity Detector Build Info ─────────────"
	@echo "Compiler:        $(CXX)"
	@echo "Flags:           $(CXXFLAGS)"
	@echo "Include:         $(INCLUDE)"
	@echo "Executable:      $(EXECUTABLE)"
	@echo "Object files:    $(ALL_OBJS)"
	@echo "──────────────────────────────────────────────────────"

# Rebuild from scratch
.PHONY: rebuild
rebuild: clean all
