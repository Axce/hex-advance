#ifndef PLAYERSPRITE_H
#define PLAYERSPRITE_H

#include <tonc.h>

#include "toolbox.h"

#define PLAYER1_SPAWN_X 176
#define PLAYER1_SPAWN_Y 100
#define PLAYER2_SPAWN_X 0
#define PLAYER2_SPAWN_Y 100

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

#endif  /* !PLAYERSPRITE_H */