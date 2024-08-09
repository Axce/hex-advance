#include "toolbox.h"
#include "PlayerSprite.h"
#include "mem_management.h"
#include "game.h"

// for 13x13
int PLAYER1_SPAWN_X = 176;
int PLAYER1_SPAWN_Y = 100;
int PLAYER2_SPAWN_X = 0;
int PLAYER2_SPAWN_Y = 100;


PlayerSprite bee = {
	&obj_buffer[OAM_BEE],
	4, 4,
	5,
	4,
	WEST,
	BEE_IDLE,
	0, 0,
	2, // vitesse max
	0, 0
};

PlayerSprite larva = {
    &obj_buffer[OAM_LARVA],
    4, 2,
    12,
    4,
    EAST,
    0,
    SCREEN_WIDTH/2-8, SCREEN_HEIGHT/2-8,
    0,
    0, 0
};


int putting_stone_delay = 0;

void init_player_spawns()
{
    PLAYER1_SPAWN_X = 111 + 5*BOARD_SIZE;
    PLAYER1_SPAWN_Y = 61 + 3*BOARD_SIZE;
    PLAYER2_SPAWN_X = 65 - 5*BOARD_SIZE;
    PLAYER2_SPAWN_Y = 61 + 3*BOARD_SIZE;
}