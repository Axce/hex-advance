#include "toolbox.h"
#include "PlayerSprite.h"

PlayerSprite bee = {
	&obj_buffer[0],
	4, 4,
	5,
	4,
	WEST,
	BEE_IDLE,
	PLAYER1_SPAWN_X, PLAYER1_SPAWN_Y,
	2, // vitesse max
	0, 0
};

int putting_stone_delay = 0;
