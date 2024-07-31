#include "coordinates.h"

extern int board[11][11];

extern Player winner;

void play();

void end_turn();

bool put_stone(Player player, Board_XY Board_XY);

void switch_player();

Board_XY* reconstruct_path(Board_XY end, Board_XY parents[11][11]);

Board_XY* has_won(Player player);

void restart_game();