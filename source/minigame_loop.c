#include <tonc.h>
#include <string.h>
#include "graphics.h"
#include "game_loop.h"
#include "game.h"
#include "movement.h"
#include "title_screen_loop.h"
#include "minigame.h"
#include "minigame_loop.h"

void init_minigame_loop()
{
	// sprites
	// bee
	memcpy(&tile_mem[4][0], bee32Tiles, bee32TilesLen/3/16); // on ne prend qu'un sprite dans la sheet de 16 sprites * 3 animations
	memcpy(pal_obj_mem, bee32Pal, bee32PalLen);
	// ghost_stone
	memcpy(&tile_mem[4][64], stoneblackTiles, stoneblackTilesLen);
	memcpy(&pal_obj_bank[1], stoneblackPal, stoneblackPalLen);
	// larva
	memcpy(&tile_mem[4][16], larvaTiles, larvaTilesLen/4);
	GRIT_CPY(&pal_obj_bank[4], larvaPal);

	oam_init(obj_buffer, OBJ_COUNT);

	// background
	memcpy(pal_bg_bank[0], hexPal, hexPalLen);		// BG_PAL 0
	memcpy(&tile_mem[0][0], hexTiles, hexTilesLen);	// CBB 0
	memcpy(&se_mem[30][0], hexMap, hexMapLen);		// SBB 30-31

	REG_BG0CNT= BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x32;
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;


	obj_set_attr(bee.obj,
		ATTR0_SQUARE,				// Square, regular sprite
		ATTR1_SIZE_32x32,			// 32*32p,
		ATTR2_PALBANK(0) | 0);		// palbank 0, tile 0

	obj_set_attr(larva.obj,
		ATTR0_SQUARE,			
		ATTR1_SIZE_16x16,			
		ATTR2_PALBANK(4) | 16);	

	obj_set_attr(ghost_stone,
		ATTR0_SQUARE,
		ATTR1_SIZE_16x16,
		ATTR2_PALBANK(1) | 64);

	init_stones_sprites();

	restart_minigame();

	game_state = MINIGAME;
}

void minigame_loop()
{
	if (key_hit(KEY_START)) {
		init_title_screen();
		return;
	}


	if (!minigame_lost && !minigame_won)
	{
		evaluate_movement();
		
		if (key_hit(KEY_A))
		{
			minigame_player_play();
		}

		update_larva_sprite();
		update_bee_sprite();

		display_ghost_stone();
	}
	
	check_lose();
}

void minigame_putting_stone_loop() 
{
    bee.current_animation = BEE_ATTACK;
	memcpy(&tile_mem[4][0], &bee32Tiles[(get_sprite_frame_1D(&bee, 20 - putting_stone_delay)) * 8] /*one 4bpp tile = 8 ints*/, bee32TilesLen/16/3);
	display_ghost_stone();
	
	if (--putting_stone_delay == 0)
	{
		obj_hide(ghost_stone);
		bee.current_animation = BEE_IDLE;
		minigame_end_turn();
	}
}
