#pragma once

enum MODE_1_OR_2_PLAYERS {
    MODE_1_PLAYER,
    MODE_2_PLAYERS,
};

extern int mode_1_or_2_players;

extern int board_bg_vofs;

void init_game_loop();

void game_loop();
void game_loop_1p_vs_cpu();
void game_loop_2p();

void game_ended_loop();

void putting_stone_loop();


extern int ofsx, ofsy;