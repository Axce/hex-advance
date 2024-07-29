#pragma once

//graphics
#include "bee.h"
#include "beedark.h"
#include "hex.h"
#include "stonewhite.h"
#include "stoneblack.h"

extern OBJ_ATTR* ghost_stone;
extern int global_frame;

// in tiles
int get_sprite_frame_2D(PlayerSprite* sprite, int global_frame);

// in tiles
int get_sprite_frame_1D(PlayerSprite* sprite, int global_frame);

void switch_player_graphics();

void display_ghost_stone();

void update_sprites();