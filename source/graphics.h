#pragma once

//graphics
#include "bee32.h"
#include "beedark.h"
#include "bee32dark.h"
#include "hex.h"
#include "stonewhite.h"
#include "stoneblack.h"
#include "coordinates.h"
#include "stonesetblack.h"
#include "stonesetwhite.h"

#include "txt_p1wins.h"
#include "txt_p2wins.h"

extern OBJ_ATTR* ghost_stone;
extern int global_frame;

// in tiles
int get_sprite_frame_2D(PlayerSprite* sprite, int global_frame);

// in tiles
int get_sprite_frame_1D(PlayerSprite* sprite, int global_frame);

void switch_player_graphics();

void display_ghost_stone();

void update_bee_sprite();

void init_stones_sprites();

void update_stones_sprites(Player player, Board_XY stone_board_pos);

void display_victory();