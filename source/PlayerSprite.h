#pragma once

#include <tonc.h>

#include "toolbox.h"

#define PLAYER1_SPAWN_X 176
#define PLAYER1_SPAWN_Y 100
#define PLAYER2_SPAWN_X 0
#define PLAYER2_SPAWN_Y 100

enum PLAYERS {
	PLAYER_1,	// yellow bee, black stones
	PLAYER_2	// red bee, white stones
};

typedef struct {
	OBJ_ATTR* obj;
	int width, height;	//in tiles
	int anim_delay;		//in frames
	int anim_frames;
	int orientation;
	int x,y;
	int max_speed;
	int x_speed, y_speed;
} PlayerSprite;

extern PlayerSprite bee;

extern int player;