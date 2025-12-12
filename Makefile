# Project configuration
PROJECT := ccraft
SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := obj

# Version configuration
VERSION_FILE := VERSION
VERSION_HEADER := $(SRC_DIR)/version.hpp

# Read or initialize version
ifeq ($(wildcard $(VERSION_FILE)),)
    MAJOR := 0
    MINOR := 0
    PATCH := 0
else
    VERSION_CONTENT := $(shell cat $(VERSION_FILE))
    MAJOR := $(word 1,$(subst ., ,$(VERSION_CONTENT)))
    MINOR := $(word 2,$(subst ., ,$(VERSION_CONTENT)))
    PATCH := $(word 3,$(subst ., ,$(VERSION_CONTENT)))
endif

# Get commit count since last patch tag
LAST_PATCH_TAG := v$(MAJOR).$(MINOR).$(PATCH)
# Try to find the tag, if it doesn't exist, count all commits
ifeq ($(shell git rev-parse $(LAST_PATCH_TAG) 2>/dev/null),)
    # Tag doesn't exist, count all commits (unreleased development)
    COMMIT_COUNT := $(shell git rev-list --count HEAD 2>/dev/null || echo 0)
else
    # Tag exists, count commits since that tag
    COMMIT_COUNT := $(shell git rev-list --count $(LAST_PATCH_TAG)..HEAD 2>/dev/null || echo 0)
endif
VERSION := $(MAJOR).$(MINOR).$(PATCH).$(COMMIT_COUNT)

# Compiler settings
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -DVERSION=\"$(VERSION)\"
LDFLAGS := 

# Source files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Target architectures
TARGETS := x86_64-linux aarch64-android

# Default target
.PHONY: all
all: version-header $(addprefix $(BIN_DIR)/$(PROJECT)-,$(TARGETS))

# Generate version header
.PHONY: version-header
version-header:
	@mkdir -p $(SRC_DIR)
	@echo "// Auto-generated version header" > $(VERSION_HEADER)
	@echo "#ifndef VERSION_HPP" >> $(VERSION_HEADER)
	@echo "#define VERSION_HPP" >> $(VERSION_HEADER)
	@echo "" >> $(VERSION_HEADER)
	@echo "#define VERSION \"$(VERSION)\"" >> $(VERSION_HEADER)
	@echo "#define VERSION_MAJOR $(MAJOR)" >> $(VERSION_HEADER)
	@echo "#define VERSION_MINOR $(MINOR)" >> $(VERSION_HEADER)
	@echo "#define VERSION_PATCH $(PATCH)" >> $(VERSION_HEADER)
	@echo "#define VERSION_COMMIT $(COMMIT_COUNT)" >> $(VERSION_HEADER)
	@echo "" >> $(VERSION_HEADER)
	@echo "#endif // VERSION_HPP" >> $(VERSION_HEADER)
	@echo "Generated $(VERSION_HEADER) with version $(VERSION)"

# Build for x86_64 Linux
$(BIN_DIR)/$(PROJECT)-x86_64-linux: $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built $@ (version $(VERSION))"

# Build for aarch64 Android (requires cross-compiler)
$(BIN_DIR)/$(PROJECT)-aarch64-android: $(SOURCES)
	@mkdir -p $(BIN_DIR)
	@if command -v aarch64-linux-android-clang++ >/dev/null 2>&1; then \
		aarch64-linux-android-clang++ -std=c++17 -O2 -DVERSION=\"$(VERSION)\" -o $@ $^; \
		echo "Built $@ (version $(VERSION))"; \
	else \
		echo "Warning: aarch64-linux-android-clang++ not found, skipping Android build"; \
	fi

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Version management targets
.PHONY: version
version:
	@echo "Current version: $(VERSION)"
	@echo "  MAJOR: $(MAJOR)"
	@echo "  MINOR: $(MINOR)"
	@echo "  PATCH: $(PATCH)"
	@echo "  COMMIT: $(COMMIT_COUNT)"

.PHONY: bump-major
bump-major:
	@echo "$$(($(MAJOR) + 1)).0.0" > $(VERSION_FILE)
	@git tag -a "v$$(($(MAJOR) + 1)).0.0" -m "Release version $$(($(MAJOR) + 1)).0.0"
	@echo "Bumped to version $$(($(MAJOR) + 1)).0.0"
	@echo "Don't forget to: git push && git push --tags"

.PHONY: bump-minor
bump-minor:
	@echo "$(MAJOR).$$(($(MINOR) + 1)).0" > $(VERSION_FILE)
	@git tag -a "v$(MAJOR).$$(($(MINOR) + 1)).0" -m "Release version $(MAJOR).$$(($(MINOR) + 1)).0"
	@echo "Bumped to version $(MAJOR).$$(($(MINOR) + 1)).0"
	@echo "Don't forget to: git push && git push --tags"

.PHONY: bump-patch
bump-patch:
	@echo "$(MAJOR).$(MINOR).$$(($(PATCH) + 1))" > $(VERSION_FILE)
	@git tag -a "v$(MAJOR).$(MINOR).$$(($(PATCH) + 1))" -m "Release version $(MAJOR).$(MINOR).$$(($(PATCH) + 1))"
	@echo "Bumped to version $(MAJOR).$(MINOR).$$(($(PATCH) + 1))"
	@echo "Don't forget to: git push && git push --tags"

# Clean targets
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)
	rm -f $(VERSION_HEADER)
	@echo "Cleaned build artifacts"

.PHONY: distclean
distclean: clean
	rm -rf $(BIN_DIR)
	@echo "Cleaned all build outputs"

# Help target
.PHONY: help
help:
	@echo "$(PROJECT) Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build all targets (default)"
	@echo "  version      - Show current version"
	@echo "  bump-major   - Increment major version (X.0.0)"
	@echo "  bump-minor   - Increment minor version (x.X.0)"
	@echo "  bump-patch   - Increment patch version (x.x.X)"
	@echo "  clean        - Remove object files and generated headers"
	@echo "  distclean    - Remove all build outputs"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Current version: $(VERSION)"

.PHONY: info
info:
	@echo "Project: $(PROJECT)"
	@echo "Version: $(VERSION)"
	@echo "Source dir: $(SRC_DIR)"
	@echo "Binary dir: $(BIN_DIR)"
	@echo "Object dir: $(OBJ_DIR)"
	@echo "Sources: $(SOURCES)"
	@echo "Targets: $(TARGETS)"