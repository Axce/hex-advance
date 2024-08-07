#pragma once

#include "coordinates.h"
#include "PlayerSprite.h"

#define MAX_BOARD_SIZE  13

extern int BOARD_SIZE;

// assumes board coordinates
static inline bool is_in_board(int x, int y)
{
    return x>=0 && x<BOARD_SIZE && y>=0 && y<BOARD_SIZE;
}

extern int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];

extern Player winner;

void player_play();

void cpu_play();

void end_turn();

bool put_stone(Player player, Board_XY Board_XY);

void switch_player();

Board_XY* reconstruct_path(Board_XY end, Board_XY parents[MAX_BOARD_SIZE][MAX_BOARD_SIZE]);

Board_XY* has_won(Player player);

void restart_game();

void display_victory();
