#include "PlayerSprite.h"
#include "graphics.h"
#include "game.h"
#include <string.h>

void play() {

    switch_player();
}

void switch_player() {

	player ^= 1;

    switch_player_graphics();
}

