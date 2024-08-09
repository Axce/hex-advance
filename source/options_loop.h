#pragma once

enum FIRST_MOVE
{
    FIRSTMOVE_PLAYER1,
    FIRSTMOVE_PLAYER2,
    FIRSTMOVE_RANDOM
};

extern int option_first_move;
extern bool option_music;
extern bool option_sound;


void init_options_loop();

void options_loop();