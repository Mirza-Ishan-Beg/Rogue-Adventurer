# Simple Makefile for C to WASM compilation
CC = emcc
CFLAGS = -Wall -Wextra -std=c11
SRC_DIR = src/c
BUILD_DIR = build
DIST_DIR = dist

# Source files
CORE_SOURCES = $(SRC_DIR)/core/game_logic.c
ALL_SOURCES = $(CORE_SOURCES)

# Output files
WASM_MODULE = $(BUILD_DIR)/game_core.wasm
JS_GLUE = $(BUILD_DIR)/game_core.js

# Compiler flags
WASM_FLAGS = -s WASM=1 \
             -s ALLOW_MEMORY_GROWTH=1 \
             -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'UTF8ToString']" \
             -s MODULARIZE=1 \
             -s EXPORT_NAME="'GameCore'" \
             -s EXPORTED_FUNCTIONS="['_game_init_live_location_wasm', '_game_cleanup_live_location_wasm', '_game_create_player_wasm', '_game_create_foe_wasm', '_game_create_item_wasm', '_game_add_scene_element_wasm', '_game_move_player_wasm', '_game_get_player_position_x', '_game_get_player_position_y', '_game_get_foe_count', '_game_get_item_count', '_game_get_live_location_wasm']"

.PHONY: all clean setup help prod dev

# Default target
all: setup prod

# Development build
dev: setup
	@echo "Building development WASM module..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -O1 -g $(WASM_FLAGS) $(ALL_SOURCES) -o $(WASM_MODULE)
	@echo "Development build complete: $(WASM_MODULE)"

# Production build
prod: setup
	@echo "Building production WASM module..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -O3 $(WASM_FLAGS) $(ALL_SOURCES) -o $(WASM_MODULE)
	@echo "Production build complete: $(WASM_MODULE)"

# Setup build environment
setup:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(DIST_DIR)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)/*
	rm -rf $(DIST_DIR)/*
	rm -f *.wasm *.js

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build production version (default)"
	@echo "  dev        - Build development version with debugging"
	@echo "  prod       - Build optimized production version"
	@echo "  clean      - Remove build artifacts"
	@echo "  help       - Show this help message"