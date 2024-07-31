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
	TITLE_SCREEN,
	IN_GAME,
	GAME_ENDED,
	PUTTING_STONE
} GAME_STATE;

extern GAME_STATE game_state;

inline int mod(int num, int den)
{
	int n = num % den;
	if (n < 0)
		n += den;
	return n;
}