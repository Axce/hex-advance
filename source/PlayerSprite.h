#pragma once

#include <tonc.h>

#include "toolbox.h"

extern int PLAYER1_SPAWN_X;
extern int PLAYER1_SPAWN_Y;
extern int PLAYER2_SPAWN_X;
extern int PLAYER2_SPAWN_Y;
/*
#define PLAYER_CENTER_X 87
#define PLAYER_CENTER_Y 40
*/

typedef enum {
	NOBODY,
	PLAYER_1_BLACK,	// yellow bee, black stones
	PLAYER_2_WHITE,	// red bee, white stones
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

void init_player_spawns();