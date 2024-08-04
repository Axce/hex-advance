#pragma once

#include <tonc.h>

#include "toolbox.h"

#define PLAYER1_SPAWN_X 176
#define PLAYER1_SPAWN_Y 100
#define PLAYER2_SPAWN_X 0
#define PLAYER2_SPAWN_Y 100
/*
#define PLAYER_CENTER_X 87
#define PLAYER_CENTER_Y 40
*/

typedef enum {
	PLAYER_1_BLACK = 1,	// yellow bee, black stones
	PLAYER_2_WHITE = 2,	// red bee, white stones
	LARVA
} Player;

enum BEE_ANIMATIONS {
	BEE_IDLE,
	BEE_FLYING,
	BEE_ATTACK
};

typedef struct {
	OBJ_ATTR* obj;
	int width, height;	//in tiles
	int anim_delay;		//in frames
	int anim_frames;
	int orientation;
	int current_animation;
	int x,y;
	int max_speed;
	int x_speed, y_speed;
} PlayerSprite;

extern PlayerSprite bee;
extern PlayerSprite larva;

extern int putting_stone_delay;