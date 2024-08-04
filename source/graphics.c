#include "PlayerSprite.h"
#include "graphics.h"
#include <string.h>
#include "coordinates.h"
#include "game.h"
#include "game_loop.h"

#define PAL_BLACKSET 2
#define PAL_WHITESET 3

OBJ_ATTR* ghost_stone = &obj_buffer[1];

OBJ_ATTR* stone_set_black[6][6];

OBJ_ATTR* stone_set_white[6][6];

// in tiles
// int get_sprite_frame_2D(PlayerSprite* sprite, int global_frame)
// {
// 	int sprite_frame = sprite->width * ((global_frame/sprite->anim_delay) % sprite->anim_frames);
// 	int orientation_offset = (sprite->width * sprite->height * sprite->anim_frames * sprite->orientation);
// 	return sprite_frame + orientation_offset;
// }

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
        memcpy(pal_obj_mem, bee32Pal, bee32PalLen);
        memcpy(&tile_mem[4][64], stoneblackTiles, stoneblackTilesLen);
        memcpy(&pal_obj_bank[1], stoneblackPal, stoneblackPalLen);
    }
    else
    {
        bee.orientation = EAST;
        bee.x = PLAYER2_SPAWN_X;
        bee.y = PLAYER2_SPAWN_Y;
        memcpy(pal_obj_mem, bee32darkPal, beedarkPalLen);
        memcpy(&tile_mem[4][64], stonewhiteTiles, stonewhiteTilesLen);
        memcpy(&pal_obj_bank[1], stonewhitePal, stonewhitePalLen);
    }
}

void display_ghost_stone()
{

	if (game_state == GAME_ENDED)
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
	//bee.obj->attr2 = ATTR2_PALBANK(0) | get_sprite_frame_2D(&bee, global_frame);
	memcpy(&tile_mem[4][0], &bee32Tiles[(get_sprite_frame_1D(&bee, global_frame)) * 8] /*one 4bpp tile = 8 ints*/, bee32TilesLen/16/3);
	obj_set_pos(bee.obj, bee.x+16, bee.y+16);	// compense le fait que le sprite fut en 64*64 avant
}

void update_larva_sprite()
{
	//bee.obj->attr2 = ATTR2_PALBANK(0) | get_sprite_frame_2D(&bee, global_frame);
	memcpy(&tile_mem[4][16], &larvaTiles[(get_sprite_frame_1D(&larva, global_frame)) * 8] /*one 4bpp tile = 8 ints*/, larvaTilesLen/4);
	obj_set_pos(larva.obj, larva.x, larva.y);
}

void init_stones_sprites()
{

	memcpy(&tile_mem[4][68], stonesetblackTiles, stonesetblackTilesLen);
	memcpy(&pal_obj_bank[PAL_BLACKSET], stonesetblackPal, stonesetblackPalLen);
	memcpy(&pal_obj_bank[PAL_WHITESET], stonesetwhitePal, stonesetwhitePalLen);

	int stone_set_id = 0;

	for (int iy = 0 ; iy < 6 ; iy++)
	{
		for (int ix = 0 ; ix < 6 ; ix++)
		{
			stone_set_black[iy][ix] = &obj_buffer[stone_set_id+2];
			stone_set_white[iy][ix] = &obj_buffer[stone_set_id+38];

			Board_XY sprite_board_pos;
			sprite_board_pos.x = ix*2;
			sprite_board_pos.y = iy*2;
			
			Screen_XY sprite_screen_pos = to_screen_xy(sprite_board_pos);
			sprite_screen_pos.x -= 8; // offset for a sprite of four stones
			sprite_screen_pos.y -= 2;  // offset for a sprite of four stones

			obj_set_attr(stone_set_black[iy][ix],
				ATTR0_SQUARE,
				ATTR1_SIZE_32x32,
				ATTR2_PALBANK(PAL_BLACKSET) | ATTR2_ID(68/*+16*15*/));
			obj_set_pos(stone_set_black[iy][ix], sprite_screen_pos.x, sprite_screen_pos.y);

			obj_set_attr(stone_set_white[iy][ix],
				ATTR0_SQUARE,
				ATTR1_SIZE_32x32,
				ATTR2_PALBANK(PAL_WHITESET) | ATTR2_ID(68/*+16*15*/));
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
	if (						  board[y0][x0] == player)	sprite_number += 1;
	if ((x1) < 11 &&    		  board[y0][x1] == player)	sprite_number += 2;
	if (			 (y1) < 11 && board[y1][x0] == player)	sprite_number += 4;
	if ((x1) < 11 && (y1) < 11 && board[y1][x1] == player)	sprite_number += 8;

	if (player == PLAYER_1_BLACK)
	{
		BFN_SET(stone_set_black[y][x]->attr2, 68+16*sprite_number, ATTR2_ID);
	}
	else
	{
		BFN_SET(stone_set_white[y][x]->attr2, 68+16*sprite_number, ATTR2_ID);
	}
}

void display_victory()
{
	switch (winner)
	{
		case PLAYER_1_BLACK:
			GRIT_CPY(&tile_mem[5][0], txt_p1winsTiles);
			GRIT_CPY(&pal_obj_bank[15], txt_p1winsPal);
			break;
		case PLAYER_2_WHITE:
			GRIT_CPY(&tile_mem[5][0], txt_p2winsTiles);
			GRIT_CPY(&pal_obj_bank[15], txt_p2winsPal);
			break;
		default:
			return;
	}

	obj_hide(bee.obj);

	obj_set_attr(&obj_buffer[74],
		ATTR0_SQUARE,
		ATTR1_SIZE_64x64,
		ATTR2_PALBANK(15) | ATTR2_ID(512));
	obj_set_pos(&obj_buffer[74], 2, 2);
	obj_unhide(&obj_buffer[74], DCNT_MODE0);

}