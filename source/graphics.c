#include "PlayerSprite.h"
#include "graphics.h"
#include <string.h>
#include "coordinates.h"

extern OBJ_ATTR* ghost_stone = &obj_buffer[1];

// in tiles
int get_sprite_frame_2D(PlayerSprite* sprite, int global_frame) {
	int sprite_frame = sprite->width * ((global_frame/sprite->anim_delay) % sprite->anim_frames);
	int orientation_offset = (sprite->width * sprite->height * sprite->anim_frames * sprite->orientation);
	return sprite_frame + orientation_offset;
}

// in tiles
int get_sprite_frame_1D(PlayerSprite* sprite, int global_frame) {
	int sprite_frame = sprite->width * sprite->height * ((global_frame/sprite->anim_delay) % sprite->anim_frames);
	int orientation_offset = (sprite->width * sprite->height * sprite->anim_frames * sprite->orientation);
	return sprite_frame + orientation_offset;
}

void switch_player_graphics()
{
    if (player == PLAYER_1)
    {
        bee.orientation = WEST;
        bee.x = PLAYER1_SPAWN_X;
        bee.y = PLAYER1_SPAWN_Y;
        memcpy(pal_obj_mem, beePal, beePalLen);
        memcpy(&tile_mem[4][64], stoneblackTiles, stoneblackTilesLen);
        memcpy(&pal_obj_bank[1], stoneblackPal, stoneblackPalLen);
    }
    else
    {
        bee.orientation = EAST;
        bee.x = PLAYER2_SPAWN_X;
        bee.y = PLAYER2_SPAWN_Y;
        memcpy(pal_obj_mem, beedarkPal, beedarkPalLen);
        memcpy(&tile_mem[4][64], stonewhiteTiles, stonewhiteTilesLen);
        memcpy(&pal_obj_bank[1], stonewhitePal, stonewhitePalLen);
    }
}

void display_ghost_stone() {
	Board_XY ghost_coordinates;

	ghost_coordinates.x = (bee.x+37)/10*10 - 8;  //adjust de +32px (moitié de largeur de bee) et -8px (moitié de largeur de ghost), et puis fine-tuning

	if ((ghost_coordinates.x/10) & 1)
		ghost_coordinates.y = (bee.y+32)/12*12 - 8 + 6 + 3;
	else
		ghost_coordinates.y = (bee.y+32+6)/12*12 - 8 + 3;
	

	if (is_stone_in_board(ghost_coordinates)) {
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

void update_sprites() {
	//bee.obj->attr2 = ATTR2_PALBANK(0) | get_sprite_frame_2D(&bee, global_frame);
	memcpy(&tile_mem[4][0], beeTiles + (get_sprite_frame_1D(&bee, global_frame)) * 8 /*one 4bpp tile = 8 ints*/, beeTilesLen/16);
	obj_set_pos(bee.obj, bee.x, bee.y);
}
