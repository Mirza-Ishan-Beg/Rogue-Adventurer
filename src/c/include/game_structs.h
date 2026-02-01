#ifndef GAME_STRUCTS_H
#define GAME_STRUCTS_H

#include <stdbool.h>
#include <stdint.h>

// Error codes
typedef enum {
    GAME_SUCCESS = 0,
    GAME_ERROR_NULL_POINTER = -1,
    GAME_ERROR_MEMORY_ALLOCATION = -2,
    GAME_ERROR_INVALID_PARAMETER = -3,
    GAME_ERROR_ENTITY_NOT_FOUND = -4,
    GAME_ERROR_SCENE_FULL = -5
} GameError;

// Basic 2D coordinate system
typedef struct {
    int x;
    int y;
} Vector2D;

// Foe entity structure
typedef struct {
    char* name;
    char* description;
    char* image_file;
    char* asset_file;
    Vector2D position;
    int health;
    int attack;
    int defense;
    bool is_alive;
} Foe;

// Item entity structure
typedef struct {
    char* name;
    char* description;
    char* image_file;
    char* asset_file;
    Vector2D position;
    int health_bonus;
    int attack_bonus;
    int defense_bonus;
    bool is_available;
} Item;

// Player entity structure
typedef struct {
    char* name;
    char* description;
    char* image_file;
    char* asset_file;
    Vector2D position;
    int health;
    int attack;
    int defense;
} Player;

// Map size structure
typedef struct {
    int width;
    int height;
} MapSize;

// Scene size structure
typedef struct {
    int width;
    int height;
} SceneSize;

// Scene element structure
typedef struct {
    char* background_asset;
    Vector2D canvas_position;
} SceneElement;

// Scene structure
typedef struct {
    SceneElement* elements;
    int element_count;
    SceneSize size;
} Scene;

// Live location tracking singleton
typedef struct {
    Player* player;
    Foe* foes;
    int foe_count;
    Item* items;
    int item_count;
    MapSize map_size;
    Scene current_scene;
} LiveLocation;

#endif // GAME_STRUCTS_H