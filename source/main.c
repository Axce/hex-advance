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

	REG_BG0CNT= BG_PRIO(1) | BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x32;
	REG_BG1CNT= BG_PRIO(0);
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;


	obj_set_attr(bee.obj,
		ATTR0_SQUARE,				// Square, regular sprite
		ATTR1_SIZE_64,				// 64x64p,
		ATTR2_PALBANK(0) | 0);		// palbank 0, tile 0

	obj_set_attr(ghost_stone,
		ATTR0_SQUARE,
		ATTR1_SIZE_16x16,
		ATTR2_PALBANK(1) | 64);

	init_stones_sprites();

	tte_init_chr4c(1, 			// BG 1
		BG_CBB(1)|BG_SBB(31),	// Charblock 1; screenblock 31
		0xF000,					// Screen-entry offset
		bytes2word(1,2,3,4),	// Color attributes.
		CLR_BLACK,	 			// Blue text
		&verdana9Font,			// Verdana 9 font
		NULL					// Use default chr4 renderer
	);

	// pal_bg_bank[15][0] = CLR_WHITE;
	// pal_bg_bank[15][1] = CLR_WHITE;
	pal_bg_bank[15][2] = CLR_WHITE;
	pal_bg_bank[15][3] = CLR_WHITE;
	pal_bg_bank[15][4] = CLR_WHITE;
	pal_bg_bank[15][5] = CLR_WHITE;

	tte_init_con();


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
