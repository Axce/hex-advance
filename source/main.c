#include <tonc.h>
#include <string.h>

#include "coordinates.h"
#include "movement.h"
#include "toolbox.h"
#include "PlayerSprite.h"

//graphics
#include "bee.h"
#include "beedark.h"
#include "hex.h"
#include "stonewhite.h"
#include "stoneblack.h"

/*
#define PLAYER_CENTER_X 87
#define PLAYER_CENTER_Y 40
*/



enum PLAYERS {
	PLAYER_1,	// yellow bee, black stones
	PLAYER_2	// red bee, white stones
};

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




int global_frame = 0;
int player = PLAYER_1;

OBJ_ATTR obj_buffer[OBJ_COUNT];

OBJ_ATTR* ghost_stone = &obj_buffer[1];

void switch_player() {

	player ^= 1;


	if (player == PLAYER_1) {
		bee.orientation = WEST;
		bee.x = PLAYER1_SPAWN_X;
		bee.y = PLAYER1_SPAWN_Y;
		memcpy(pal_obj_mem, beePal, beePalLen);
		memcpy(&tile_mem[4][64], stoneblackTiles, stoneblackTilesLen);
		memcpy(&pal_obj_bank[1], stoneblackPal, stoneblackPalLen);
	}
	else {
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
	
	obj_set_pos(ghost_stone, ghost_coordinates.x, ghost_coordinates.y);

	if (is_stone_in_board(ghost_coordinates)) {
		ghost_stone->attr0 ^= ATTR0_HIDE;
	}
	else {
		obj_hide(ghost_stone);
	}
}

void update_sprites() {
	//bee.obj->attr2 = ATTR2_PALBANK(0) | get_sprite_frame_2D(&bee, global_frame);
	memcpy(&tile_mem[4][0], beeTiles + (get_sprite_frame_1D(&bee, global_frame)) * 8 /*one 4bpp tile = 8 ints*/, beeTilesLen/16);
	obj_set_pos(bee.obj, bee.x, bee.y);
}

void play() {
	switch_player();
}

int main()
{

	memcpy(&tile_mem[4][0], beeTiles, beeTilesLen/16); // on ne prend qu'un sprite dans la sheet de 16
	memcpy(pal_obj_mem, beePal, beePalLen);
	memcpy(&tile_mem[4][64], stoneblackTiles, stoneblackTilesLen);
	memcpy(&pal_obj_bank[1], stoneblackPal, stoneblackPalLen);

	

	oam_init(obj_buffer, OBJ_COUNT);

	// Load palette
	memcpy(pal_bg_mem, hexPal, hexPalLen);
	// Load tiles into CBB 0
	memcpy(&tile_mem[0][0], hexTiles, hexTilesLen);
	// Load map into SBB 30
	memcpy(&se_mem[30][0], hexMap, hexMapLen);

	REG_BG0CNT= BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x32;
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;


	obj_set_attr(bee.obj,
		ATTR0_SQUARE,				// Square, regular sprite
		ATTR1_SIZE_64,				// 64x64p,
		ATTR2_PALBANK(0) | 0);		// palbank 0, tile 0

	obj_set_attr(ghost_stone,
		ATTR0_SQUARE,
		ATTR1_SIZE_16x16,
		ATTR2_PALBANK(1) | 64);

	while(1)
	{

		vid_vsync();		
		
		global_frame++;

		key_poll();

		if (key_hit(KEY_A)) {
			play();
		}
		
		evaluate_movement();

		display_ghost_stone();
		
		update_sprites();

		obj_copy(obj_mem, obj_buffer, OBJ_COUNT);
	}

	return 0;
}
