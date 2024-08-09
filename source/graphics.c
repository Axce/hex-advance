#include "PlayerSprite.h"
#include "graphics.h"
#include <string.h>
#include "coordinates.h"
#include "game.h"
#include "game_loop.h"
#include "minigame.h"
#include "mem_management.h"

#define SET_SIZE	7

OBJ_ATTR* ghost_stone = &obj_buffer[OAM_GHOST_STONE];

OBJ_ATTR* stone_set_black[SET_SIZE][SET_SIZE];

OBJ_ATTR* stone_set_white[SET_SIZE][SET_SIZE];

// in tiles
int get_sprite_frame_1D(PlayerSprite* sprite, int global_frame)
{
	int sprite_frame = sprite->width * sprite->height * ((global_frame/sprite->anim_delay) % sprite->anim_frames);
	int orientation_offset = sprite->width * sprite->height * sprite->anim_frames * sprite->orientation;
	int animation_offset = sprite->width * sprite->height * sprite->anim_frames * 4 * sprite->current_animation;
	return sprite_frame + orientation_offset + animation_offset;
}

void switch_player_graphics()
{
    if (current_player == PLAYER_1_BLACK)
    {
        bee.orientation = WEST;
        bee.x = PLAYER1_SPAWN_X;
        bee.y = PLAYER1_SPAWN_Y;
        GRIT_CPY(&pal_obj_bank[PAL_BEE], bee32Pal);
        GRIT_CPY(&tile_mem_obj_tile[TILE_GHOST_STONE], stoneblackTiles);
        GRIT_CPY(&pal_obj_bank[PAL_GHOST_STONE], stoneblackPal);
    }
    else
    {
        bee.orientation = EAST;
		if (mode_1_or_2_players == MODE_2_PLAYERS)
		{
        	bee.x = PLAYER2_SPAWN_X;
        	bee.y = PLAYER2_SPAWN_Y;
		}
		else	// just for preventing bee appearing just for 1 frame at spawn
		{
        	bee.x = -64;
        	bee.y = -64;			
		}
        GRIT_CPY(&pal_obj_bank[PAL_BEE], bee32darkPal);
        GRIT_CPY(&tile_mem_obj_tile[TILE_GHOST_STONE], stonewhiteTiles);
        GRIT_CPY(&pal_obj_bank[PAL_GHOST_STONE], stonewhitePal);
    }
}

void display_ghost_stone()
{

	if (game_state == GAME_ENDED || game_state == MINIGAME_ENDED)
	{
		obj_hide(ghost_stone);
		return;
	}

	Screen_XY ghost_coordinates;

	if (mode_1_or_2_players == MODE_1_PLAYER && current_player == PLAYER_2_WHITE)
	{
		ghost_coordinates = new_screen_xy(bee.x+32, bee.y+32);
	}
	else
	{
		ghost_coordinates.x = (bee.x+37)/10*10 - 8;  //adjust de +32px (moitié de largeur de bee) et -8px (moitié de largeur de ghost), et puis fine-tuning

		if ((ghost_coordinates.x/10) & 1)
			ghost_coordinates.y = (bee.y+32)/12*12 - 8 + 6 + 3;
		else
			ghost_coordinates.y = (bee.y+32+6)/12*12 - 8 + 3;
	}

	if (is_stone_in_board(ghost_coordinates))
	{
	    obj_set_pos(ghost_stone, ghost_coordinates.x, ghost_coordinates.y);
		ghost_stone->attr0 ^= ATTR0_HIDE;
	}
	else {
        ghost_coordinates.x = bee.x + 24;

        ghost_coordinates.y = bee.y + 23;

	    obj_set_pos(ghost_stone, ghost_coordinates.x, ghost_coordinates.y);
		obj_unhide(ghost_stone, DCNT_MODE0);
	}
}

void update_bee_sprite()
{
	memcpy(&tile_mem_obj_tile[TILE_BEE], &bee32Tiles[(get_sprite_frame_1D(&bee, global_frame)) * 8] /*one 4bpp tile = 8 ints*/, bee32TilesLen/16/3);
	obj_set_pos(bee.obj, bee.x+16, bee.y+16);	// compense le fait que le sprite fut en 64*64 avant
}

void update_larva_sprite()
{
	memcpy(&tile_mem_obj_tile[TILE_LARVA], &larvaTiles[(get_sprite_frame_1D(&larva, global_frame)) * 8] /*one 4bpp tile = 8 ints*/, larvaTilesLen/4);
	
	Screen_XY larva_screen_pos = to_screen_xy(larva_board_xy);
    larva.x = larva_screen_pos.x;
    larva.y = larva_screen_pos.y;
	obj_set_pos(larva.obj, larva.x, larva.y);
}

void init_stones_sprites()
{

	GRIT_CPY(&tile_mem_obj_tile[TILE_STONESET], stonesetblackTiles);
	GRIT_CPY(&pal_obj_bank[PAL_STONESET_BLACK], stonesetblackPal);
	GRIT_CPY(&pal_obj_bank[PAL_STONESET_WHITE], stonesetwhitePal);

	int stone_set_id = 0;

	for (int iy = 0 ; iy < SET_SIZE ; iy++)
	{
		for (int ix = 0 ; ix < SET_SIZE ; ix++)
		{
			stone_set_black[iy][ix] = &obj_buffer[stone_set_id+OAM_STONESET_FIRSTBLACK];
			stone_set_white[iy][ix] = &obj_buffer[stone_set_id+OAM_STONESET_FIRSTWHITE];

			Board_XY sprite_board_pos;
			sprite_board_pos.x = ix*2;
			sprite_board_pos.y = iy*2;
			
			Screen_XY sprite_screen_pos = to_screen_xy(sprite_board_pos);
			sprite_screen_pos.x -= 8; // offset for a sprite of four stones
			sprite_screen_pos.y -= 2;  // offset for a sprite of four stones

			obj_set_attr(stone_set_black[iy][ix],
				ATTR0_SQUARE,
				ATTR1_SIZE_32x32,
				ATTR2_PALBANK(PAL_STONESET_BLACK) | ATTR2_ID(TILE_STONESET));
			obj_set_pos(stone_set_black[iy][ix], sprite_screen_pos.x, sprite_screen_pos.y);

			obj_set_attr(stone_set_white[iy][ix],
				ATTR0_SQUARE,
				ATTR1_SIZE_32x32,
				ATTR2_PALBANK(PAL_STONESET_WHITE) | ATTR2_ID(TILE_STONESET));
			obj_set_pos(stone_set_white[iy][ix], sprite_screen_pos.x, sprite_screen_pos.y);
			
			stone_set_id++;
		}
	}
}

void update_stones_sprites(Player player, Board_XY stone_board_pos)
{

	// sprite (stone_set) xy
	int x = stone_board_pos.x / 2;
	int y = stone_board_pos.y / 2;

	// concerned stones board pos
	int x0 = 2*x;
	int x1 = 2*x+1;
	int y0 = 2*y;
	int y1 = 2*y+1;
	
	int sprite_number = 0;
	if (						  				  board[y0][x0] == player)	sprite_number += 1;
	if ((x1) < BOARD_SIZE &&    		  		  board[y0][x1] == player)	sprite_number += 2;
	if (			 		 (y1) < BOARD_SIZE && board[y1][x0] == player)	sprite_number += 4;
	if ((x1) < BOARD_SIZE && (y1) < BOARD_SIZE && board[y1][x1] == player)	sprite_number += 8;

	if (player == PLAYER_1_BLACK)
	{
		BFN_SET(stone_set_black[y][x]->attr2, TILE_STONESET + 16*sprite_number, ATTR2_ID);
	}
	else
	{
		BFN_SET(stone_set_white[y][x]->attr2, TILE_STONESET + 16*sprite_number, ATTR2_ID);
	}
}

