#pragma once

#include "coordinates.h"
#include "PlayerSprite.h"

#define BOARD_SIZE  11

// assumes board coordinates
inline bool is_in_board(int x, int y)
{
    return x>=0 && x<BOARD_SIZE && y>=0 && y<BOARD_SIZE;
}

extern int board[11][11];

extern Player winner;

void player_play();

void cpu_play();

void end_turn();

bool put_stone(Player player, Board_XY Board_XY);

void switch_player();

Board_XY* reconstruct_path(Board_XY end, Board_XY parents[11][11]);

Board_XY* has_won(Player player);

void restart_game();

void display_victory();
