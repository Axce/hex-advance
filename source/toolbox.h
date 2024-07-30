#pragma once

#include <tonc.h>

#define OBJ_COUNT 128

extern OBJ_ATTR obj_buffer[OBJ_COUNT];

enum ORIENTATIONS {
	NORTH,
	WEST,
	EAST,
	SOUTH
};

typedef enum {
	IN_GAME

} GAME_STATE;

extern GAME_STATE game_state;