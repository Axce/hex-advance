#pragma once

//graphics
#include "bee32.h"
#include "beedark.h"
#include "bee32dark.h"
#include "board3.h"
#include "board5.h"
#include "board7.h"
#include "board9.h"
#include "board11.h"
#include "board13.h"
#include "board_bg.h"
#include "stonewhite.h"
#include "stoneblack.h"
#include "coordinates.h"
#include "stonesetblack.h"
#include "stonesetwhite.h"
#include "larva.h"

//menus
#include "menu_cursor.h"
#include "menu_gameover.h"
#include "menu_p1wins.h"
#include "menu_p2wins.h"
#include "menu_palette.h"
#include "menu_pause.h"
#include "menu_youwin.h"


//title screen
#include "title_bg.h"
#include "title_beekeeper.h"
#include "title_hexadvance.h"
#include "title_menutext.h"
#include "menu_options.h"
#include "menu_cursor_bs.h"
#include "menu_cursor_fm.h"
#include "tutorial.h"

#include "trans_black.h"
#include "trans_side.h"

#include "PlayerSprite.h"

#include "gbajam.h"

extern OBJ_ATTR* ghost_stone;

extern bool is_stone_puttable;


// in tiles
int get_sprite_frame_2D(PlayerSprite* sprite, int global_frame);

// in tiles
int get_sprite_frame_1D(PlayerSprite* sprite, int global_frame);

void switch_player_graphics();

void display_ghost_stone();

void update_bee_sprite();

void update_larva_sprite();

void init_stones_sprites();

void update_stones_sprites(Player player, Board_XY stone_board_pos);
