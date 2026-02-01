#include "../include/game_structs.h"
#include <stdlib.h>
#include <string.h>

// EMSCRIPTEN_KEEPALIVE will be defined by Emscripten during compilation
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

// Global live location singleton
static LiveLocation g_liveLocation = {0};

// Initialize the live location singleton
GameError game_init_live_location(MapSize map_size, SceneSize scene_size) {
    g_liveLocation.player = NULL;
    g_liveLocation.foes = NULL;
    g_liveLocation.foe_count = 0;
    g_liveLocation.items = NULL;
    g_liveLocation.item_count = 0;
    g_liveLocation.map_size = map_size;
    g_liveLocation.current_scene.size = scene_size;
    g_liveLocation.current_scene.elements = NULL;
    g_liveLocation.current_scene.element_count = 0;
    
    return GAME_SUCCESS;
}

// Clean up live location singleton
void game_cleanup_live_location() {
    if (g_liveLocation.player) {
        free(g_liveLocation.player);
        g_liveLocation.player = NULL;
    }
    
    if (g_liveLocation.foes) {
        for (int i = 0; i < g_liveLocation.foe_count; i++) {
            if (g_liveLocation.foes[i].name) free(g_liveLocation.foes[i].name);
            if (g_liveLocation.foes[i].description) free(g_liveLocation.foes[i].description);
            if (g_liveLocation.foes[i].image_file) free(g_liveLocation.foes[i].image_file);
            if (g_liveLocation.foes[i].asset_file) free(g_liveLocation.foes[i].asset_file);
        }
        free(g_liveLocation.foes);
        g_liveLocation.foes = NULL;
        g_liveLocation.foe_count = 0;
    }
    
    if (g_liveLocation.items) {
        for (int i = 0; i < g_liveLocation.item_count; i++) {
            if (g_liveLocation.items[i].name) free(g_liveLocation.items[i].name);
            if (g_liveLocation.items[i].description) free(g_liveLocation.items[i].description);
            if (g_liveLocation.items[i].image_file) free(g_liveLocation.items[i].image_file);
            if (g_liveLocation.items[i].asset_file) free(g_liveLocation.items[i].asset_file);
        }
        free(g_liveLocation.items);
        g_liveLocation.items = NULL;
        g_liveLocation.item_count = 0;
    }
    
    if (g_liveLocation.current_scene.elements) {
        for (int i = 0; i < g_liveLocation.current_scene.element_count; i++) {
            if (g_liveLocation.current_scene.elements[i].background_asset) {
                free(g_liveLocation.current_scene.elements[i].background_asset);
            }
        }
        free(g_liveLocation.current_scene.elements);
        g_liveLocation.current_scene.elements = NULL;
        g_liveLocation.current_scene.element_count = 0;
    }
}

// Create a new player
GameError game_create_player(const char* name, const char* description, 
                           const char* image_file, const char* asset_file,
                           Vector2D position, int health, int attack, int defense) {
    if (!name || !description || !image_file || !asset_file) {
        return GAME_ERROR_NULL_POINTER;
    }
    
    Player* player = (Player*)malloc(sizeof(Player));
    if (!player) {
        return GAME_ERROR_MEMORY_ALLOCATION;
    }
    
    player->name = strdup(name);
    player->description = strdup(description);
    player->image_file = strdup(image_file);
    player->asset_file = strdup(asset_file);
    player->position = position;
    player->health = health;
    player->attack = attack;
    player->defense = defense;
    
    g_liveLocation.player = player;
    return GAME_SUCCESS;
}

// Create a new foe
GameError game_create_foe(const char* name, const char* description,
                         const char* image_file, const char* asset_file,
                         Vector2D position, int health, int attack, int defense, bool is_alive) {
    if (!name || !description || !image_file || !asset_file) {
        return GAME_ERROR_NULL_POINTER;
    }
    
    // Resize foes array
    Foe* new_foes = (Foe*)realloc(g_liveLocation.foes, 
                                   (g_liveLocation.foe_count + 1) * sizeof(Foe));
    if (!new_foes) {
        return GAME_ERROR_MEMORY_ALLOCATION;
    }
    
    g_liveLocation.foes = new_foes;
    
    // Add new foe
    Foe* foe = &g_liveLocation.foes[g_liveLocation.foe_count];
    foe->name = strdup(name);
    foe->description = strdup(description);
    foe->image_file = strdup(image_file);
    foe->asset_file = strdup(asset_file);
    foe->position = position;
    foe->health = health;
    foe->attack = attack;
    foe->defense = defense;
    foe->is_alive = is_alive;
    
    g_liveLocation.foe_count++;
    return GAME_SUCCESS;
}

// Create a new item
GameError game_create_item(const char* name, const char* description,
                         const char* image_file, const char* asset_file,
                         Vector2D position, int health_bonus, int attack_bonus, 
                         int defense_bonus, bool is_available) {
    if (!name || !description || !image_file || !asset_file) {
        return GAME_ERROR_NULL_POINTER;
    }
    
    // Resize items array
    Item* new_items = (Item*)realloc(g_liveLocation.items, 
                                   (g_liveLocation.item_count + 1) * sizeof(Item));
    if (!new_items) {
        return GAME_ERROR_MEMORY_ALLOCATION;
    }
    
    g_liveLocation.items = new_items;
    
    // Add new item
    Item* item = &g_liveLocation.items[g_liveLocation.item_count];
    item->name = strdup(name);
    item->description = strdup(description);
    item->image_file = strdup(image_file);
    item->asset_file = strdup(asset_file);
    item->position = position;
    item->health_bonus = health_bonus;
    item->attack_bonus = attack_bonus;
    item->defense_bonus = defense_bonus;
    item->is_available = is_available;
    
    g_liveLocation.item_count++;
    return GAME_SUCCESS;
}

// Add scene element
GameError game_add_scene_element(const char* background_asset, Vector2D canvas_position) {
    if (!background_asset) {
        return GAME_ERROR_NULL_POINTER;
    }
    
    // Resize elements array
    SceneElement* new_elements = (SceneElement*)realloc(g_liveLocation.current_scene.elements,
                                                    (g_liveLocation.current_scene.element_count + 1) * sizeof(SceneElement));
    if (!new_elements) {
        return GAME_ERROR_MEMORY_ALLOCATION;
    }
    
    g_liveLocation.current_scene.elements = new_elements;
    
    // Add new element
    SceneElement* element = &g_liveLocation.current_scene.elements[g_liveLocation.current_scene.element_count];
    element->background_asset = strdup(background_asset);
    element->canvas_position = canvas_position;
    
    g_liveLocation.current_scene.element_count++;
    return GAME_SUCCESS;
}

// Get live location singleton
LiveLocation* game_get_live_location() {
    return &g_liveLocation;
}

// Move player to new position
GameError game_move_player(Vector2D new_position) {
    if (!g_liveLocation.player) {
        return GAME_ERROR_NULL_POINTER;
    }
    
    // Check bounds
    if (new_position.x < 0 || new_position.x >= g_liveLocation.map_size.width ||
        new_position.y < 0 || new_position.y >= g_liveLocation.map_size.height) {
        return GAME_ERROR_INVALID_PARAMETER;
    }
    
    g_liveLocation.player->position = new_position;
    return GAME_SUCCESS;
}

// Get player at position
Player* game_get_player_at(Vector2D position) {
    if (!g_liveLocation.player) {
        return NULL;
    }
    
    if (g_liveLocation.player->position.x == position.x &&
        g_liveLocation.player->position.y == position.y) {
        return g_liveLocation.player;
    }
    
    return NULL;
}

// Get foe at position
Foe* game_get_foe_at(Vector2D position) {
    for (int i = 0; i < g_liveLocation.foe_count; i++) {
        if (!g_liveLocation.foes[i].is_alive) continue;
        
        if (g_liveLocation.foes[i].position.x == position.x &&
            g_liveLocation.foes[i].position.y == position.y) {
            return &g_liveLocation.foes[i];
        }
    }
    return NULL;
}

// Get item at position
Item* game_get_item_at(Vector2D position) {
    for (int i = 0; i < g_liveLocation.item_count; i++) {
        if (!g_liveLocation.items[i].is_available) continue;
        
        if (g_liveLocation.items[i].position.x == position.x &&
            g_liveLocation.items[i].position.y == position.y) {
            return &g_liveLocation.items[i];
        }
    }
    return NULL;
}

// Remove foe by index
GameError game_remove_foe(int index) {
    if (index < 0 || index >= g_liveLocation.foe_count) {
        return GAME_ERROR_INVALID_PARAMETER;
    }
    
    Foe* foe = &g_liveLocation.foes[index];
    
    // Free memory
    free(foe->name);
    free(foe->description);
    free(foe->image_file);
    free(foe->asset_file);
    
    // Shift remaining foes
    for (int i = index; i < g_liveLocation.foe_count - 1; i++) {
        g_liveLocation.foes[i] = g_liveLocation.foes[i + 1];
    }
    
    g_liveLocation.foe_count--;
    
    // Resize array
    g_liveLocation.foes = (Foe*)realloc(g_liveLocation.foes, 
                                      g_liveLocation.foe_count * sizeof(Foe));
    
    return GAME_SUCCESS;
}

// Remove item by index
GameError game_remove_item(int index) {
    if (index < 0 || index >= g_liveLocation.item_count) {
        return GAME_ERROR_INVALID_PARAMETER;
    }
    
    Item* item = &g_liveLocation.items[index];
    
    // Free memory
    free(item->name);
    free(item->description);
    free(item->image_file);
    free(item->asset_file);
    
    // Shift remaining items
    for (int i = index; i < g_liveLocation.item_count - 1; i++) {
        g_liveLocation.items[i] = g_liveLocation.items[i + 1];
    }
    
    g_liveLocation.item_count--;
    
    // Resize array
    g_liveLocation.items = (Item*)realloc(g_liveLocation.items, 
                                       g_liveLocation.item_count * sizeof(Item));
    
    return GAME_SUCCESS;
}

// WASM export functions
EMSCRIPTEN_KEEPALIVE
GameError game_init_live_location_wasm(int map_width, int map_height, int scene_width, int scene_height) {
    MapSize map_size = { map_width, map_height };
    SceneSize scene_size = { scene_width, scene_height };
    return game_init_live_location(map_size, scene_size);
}

EMSCRIPTEN_KEEPALIVE
void game_cleanup_live_location_wasm() {
    game_cleanup_live_location();
}

EMSCRIPTEN_KEEPALIVE
GameError game_create_player_wasm(const char* name, const char* description,
                                 const char* image_file, const char* asset_file,
                                 int x, int y, int health, int attack, int defense) {
    Vector2D position = { x, y };
    return game_create_player(name, description, image_file, asset_file, position, health, attack, defense);
}

EMSCRIPTEN_KEEPALIVE
GameError game_create_foe_wasm(const char* name, const char* description,
                               const char* image_file, const char* asset_file,
                               int x, int y, int health, int attack, int defense, int is_alive) {
    Vector2D position = { x, y };
    return game_create_foe(name, description, image_file, asset_file, position, health, attack, defense, (bool)is_alive);
}

EMSCRIPTEN_KEEPALIVE
GameError game_create_item_wasm(const char* name, const char* description,
                               const char* image_file, const char* asset_file,
                               int x, int y, int health_bonus, int attack_bonus, 
                               int defense_bonus, int is_available) {
    Vector2D position = { x, y };
    return game_create_item(name, description, image_file, asset_file, position, health_bonus, attack_bonus, defense_bonus, (bool)is_available);
}

EMSCRIPTEN_KEEPALIVE
GameError game_add_scene_element_wasm(const char* background_asset, int canvas_x, int canvas_y) {
    Vector2D canvas_position = { canvas_x, canvas_y };
    return game_add_scene_element(background_asset, canvas_position);
}

EMSCRIPTEN_KEEPALIVE
GameError game_move_player_wasm(int x, int y) {
    Vector2D position = { x, y };
    return game_move_player(position);
}

EMSCRIPTEN_KEEPALIVE
int game_get_player_position_x() {
    return g_liveLocation.player ? g_liveLocation.player->position.x : -1;
}

EMSCRIPTEN_KEEPALIVE
int game_get_player_position_y() {
    return g_liveLocation.player ? g_liveLocation.player->position.y : -1;
}

EMSCRIPTEN_KEEPALIVE
int game_get_foe_count() {
    return g_liveLocation.foe_count;
}

EMSCRIPTEN_KEEPALIVE
int game_get_item_count() {
    return g_liveLocation.item_count;
}

EMSCRIPTEN_KEEPALIVE
LiveLocation* game_get_live_location_wasm() {
    return &g_liveLocation;
}