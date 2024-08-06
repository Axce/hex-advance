#include <tonc.h>
#include <string.h>
#include "graphics.h"
#include "game_loop.h"
#include "game.h"
#include "movement.h"
#include "title_screen_loop.h"
#include "minigame.h"
#include "minigame_loop.h"
#include "mem_management.h"
#include "menus.h"

void init_minigame_loop()
{
	REG_BG0CNT= BG_CBB(CBB_BOARD) | BG_SBB(SBB_BOARD) | BG_4BPP | BG_REG_32x32;
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

	// background
	GRIT_CPY(pal_bg_bank[BGPAL_BOARD], board11Pal);		// BG_PAL 0
	GRIT_CPY(&tile_mem[CBB_BOARD], board11Tiles);	// CBB 0
	GRIT_CPY(&se_mem[SBB_BOARD], board11Map);		// SBB 30-31


	// sprites
	oam_init(obj_buffer, OBJ_COUNT);
	// bee
	memcpy(&tile_mem_obj_tile[TILE_BEE], bee32Tiles, bee32TilesLen/3/16); // on ne prend qu'un sprite dans la sheet de 16 sprites * 3 animations
	GRIT_CPY(pal_obj_mem, bee32Pal);
	obj_set_attr(bee.obj,
		ATTR0_SQUARE,				// Square, regular sprite
		ATTR1_SIZE_32x32,			// 32*32p,
		ATTR2_PALBANK(PAL_BEE) | TILE_BEE);		// palbank 0, tile 0

	// ghost_stone
	GRIT_CPY(&tile_mem_obj_tile[TILE_GHOST_STONE], stoneblackTiles);
	GRIT_CPY(&pal_obj_bank[PAL_GHOST_STONE], stoneblackPal);
	obj_set_attr(ghost_stone,
		ATTR0_SQUARE,
		ATTR1_SIZE_16x16,
		ATTR2_PALBANK(PAL_GHOST_STONE) | TILE_GHOST_STONE);

	// larva
	memcpy(&tile_mem_obj_tile[TILE_LARVA], larvaTiles, larvaTilesLen/4);
	GRIT_CPY(&pal_obj_bank[PAL_LARVA], larvaPal);
	obj_set_attr(larva.obj,
		ATTR0_SQUARE,			
		ATTR1_SIZE_16x16,			
		ATTR2_PALBANK(PAL_LARVA) | TILE_LARVA);	


	init_stones_sprites();

	restart_minigame();

	game_state = MINIGAME;
}

void minigame_loop()
{
	if (key_hit(KEY_START)) {
		init_menu(MENU_PAUSE);
		return;
	}

	if (minigame_lost)
		init_menu(MENU_GAMEOVER);
	// if (minigame_won)
	// 	init_menu(MENU_YOUWIN);

	evaluate_movement();
	
	if (key_hit(KEY_A))
	{
		minigame_player_play();
	}

	update_larva_sprite();
	update_bee_sprite();

	display_ghost_stone();
	
	check_lose();
}

void minigame_putting_stone_loop() 
{
    bee.current_animation = BEE_ATTACK;
	memcpy(&tile_mem_obj_tile[TILE_BEE], &bee32Tiles[(get_sprite_frame_1D(&bee, 20 - putting_stone_delay)) * 8] /*one 4bpp tile = 8 ints*/, bee32TilesLen/16/3);
	display_ghost_stone();
	
	if (--putting_stone_delay == 0)
	{
		obj_hide(ghost_stone);
		bee.current_animation = BEE_IDLE;
		minigame_end_turn();
	}
}

void minigame_ended_loop()
{
	
}