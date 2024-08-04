#pragma once

#include "coordinates.h"
#include "PlayerSprite.h"
#include "game.h"

extern Board_XY larva_board_xy;

void restart_minigame();

void minigame_player_play();

void larva_play();

Board_XY larva_find_next_move();

void larva_move(Board_XY new_pos);

void minigame_end_turn();