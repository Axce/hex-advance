#pragma once

#include "coordinates.h"
#include "game.h"

Board_XY cpu_find_next_move();

Board_XY random_ai();
Board_XY best_score_ai(int board[BOARD_SIZE][BOARD_SIZE], int player);

int least_moves_to_win(int board[BOARD_SIZE][BOARD_SIZE], int player);

bool is_free_bridge(int board[BOARD_SIZE][BOARD_SIZE], int x, int y, int ni);
bool is_a_blocking_enemy_bridge(int board[BOARD_SIZE][BOARD_SIZE], int enemy, int x, int y, int ni);
int is_owned_by(int board[BOARD_SIZE][BOARD_SIZE], int x, int y);
bool is_connected_to_end_border(int board[BOARD_SIZE][BOARD_SIZE], int player, int x, int y);
