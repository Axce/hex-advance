#include "toolbox.h"
#include "PlayerSprite.h"

PlayerSprite bee = {
	&obj_buffer[0],
	8, 8,
	5,
	4,
	WEST,
	PLAYER1_SPAWN_X, PLAYER1_SPAWN_Y,
	2, // vitesse max
	0, 0
};