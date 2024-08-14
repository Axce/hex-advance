#pragma once

#include "coordinates.h"
#include "game.h"

// directions as seen on screen 
enum DIRECT_NEIGHBORS {
    DN_TOP,
    DN_TOPRIGHT,
    DN_TOPLEFT,
    DN_BOTRIGHT,
    DN_BOTLEFT,
    DN_BOT
};
// extern const int direct_neighbors_y[6];
// extern const int direct_neighbors_x[6];
// extern const int direct_obstacle_1_y[6];
// extern const int direct_obstacle_1_x[6];
// extern const int direct_obstacle_2_y[6];
// extern const int direct_obstacle_2_x[6];

// directions as seen on screen 
enum BRIDGE_NEIGHBORS {
    BN_TOPRIGHT,
    BN_TOPLEFT,
    BN_RIGHT,
    BN_LEFT,
    BN_BOTRIGHT,
    BN_BOTLEFT
};
// extern const int bridge_neighbors_y[6];
// extern const int bridge_neighbors_x[6];
// extern const int bridge_obstacle_1_y[6];
// extern const int bridge_obstacle_1_x[6];
// extern const int bridge_obstacle_2_y[6];
// extern const int bridge_obstacle_2_x[6];

extern int thinking_progress;

extern const int get_enemy[3];

Board_XY cpu_find_next_move();

void update_bee_thinking_position();

Board_XY random_ai();
Board_XY best_score_ai(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int player);
Board_XY best_own_score_ai(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int player);
Board_XY minimax_ai(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int player);

int minimax(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int player, int beta);
int minimax_heuristic(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], Player next_player);

int least_moves_to_win(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], Player player, Player next_player);
int least_moves_to_win_fast(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], Player player, Player next_player);

bool is_free_bridge(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int x, int y, enum BRIDGE_NEIGHBORS ni);
bool is_free_ziggurat(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int x, int y, Player player, Player next_player);
bool is_blocked_by_enemy_bridge(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], Player enemy, Player next_player, int x, int y, enum DIRECT_NEIGHBORS ni);
Player is_owned_by(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int x, int y);
bool is_connected_to_end_border(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int player, int x, int y, Player next_player);
bool is_connected_to_end_border_fast(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int player, int x, int y, Player next_player);
