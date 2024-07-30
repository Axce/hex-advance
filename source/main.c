#include <tonc.h>
#include <string.h>
#include <stdio.h>

#include "coordinates.h"
#include "movement.h"
#include "toolbox.h"
#include "PlayerSprite.h"
#include "graphics.h"
#include "game.h"

int global_frame = 0;
int current_player = PLAYER_1_BLACK;
OBJ_ATTR obj_buffer[OBJ_COUNT];
GAME_STATE game_state = IN_GAME;

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

	init_stones_sprites();

	while(1)
	{

		vid_vsync();		
		
		global_frame++;

		key_poll();

		if (key_hit(KEY_A) && !winner) {
			play();
		}

		if (key_hit(KEY_START)) {
			restart_game();
		}
		
		evaluate_movement();

		if (!winner)
		{
			update_bee_sprite();
			display_ghost_stone();
		}

		obj_copy(obj_mem, obj_buffer, OBJ_COUNT);

	}

	return 0;
}
