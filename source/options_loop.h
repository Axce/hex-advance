#pragma once

enum FIRST_MOVE
{
    FIRSTMOVE_PLAYER1,
    FIRSTMOVE_PLAYER2,
    FIRSTMOVE_RANDOM
};

extern int option_first_move;

void init_options_loop();

void options_loop();