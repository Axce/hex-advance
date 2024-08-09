#pragma once

#include <tonc.h>

#define OBJ_COUNT 128

extern int global_frame;

extern int current_player;

extern OBJ_ATTR obj_buffer[OBJ_COUNT];

enum ORIENTATIONS
{
	NORTH,
	WEST,
	EAST,
	SOUTH
};

typedef enum GAME_STATE
{
	TITLE_SCREEN,
	TUTORIAL,
	OPTIONS,

	IN_GAME,
	GAME_ENDED,
	PUTTING_STONE,
	
	MINIGAME,
	MINIGAME_PUTTING_STONE,
	MINIGAME_ENDED,

	MENUING
} GAME_STATE;

extern GAME_STATE game_state;

static inline int mod(int num, int den)
{
	int n = num % den;
	if (n < 0)
		n += den;
	return n;
}

static inline int power(int n, int power)
{
    int result = 1;
	while(power--)
	{
		result *= n;
	}
	return result;
}

static inline int ease_out(int n, int max)
{
	int theta = n * 0x4000 / max;
	int j = lu_sin(theta) * max >> 12;

	return j;
}

static inline int ease_in(int n, int max)
{
	int theta = (max-n) * 0x4000 / max;
	int j = lu_cos(theta) * max >> 12;

	return j;
}

static inline int ease_in_out(int n, int max)
{
	int theta = n * 0x8000 / max;	// theta from 0 to 180°
	int j = lu_cos(theta) * max >> 13;	// cos from max to -max

	return -j + max/2;
}