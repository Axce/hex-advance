#include <tonc.h>
#include <string.h>
#include "graphics.h"
#include "game_loop.h"
#include "game.h"
#include "movement.h"
#include "title_screen_loop.h"
#include "mem_management.h"
#include "menus.h"
#include "audio.h"

int mode_1_or_2_players;

int board_bg_vofs = 0;

void init_game_loop()
{

	play_transition_in();
	
	memcpy(&tile_mem_obj_tile[TILE_BEE], bee32Tiles, bee32TilesLen/3/16); // on ne prend qu'un sprite dans la sheet de 16 sprites * 3 animations
	GRIT_CPY(&pal_obj_bank[PAL_BEE], bee32Pal);
	GRIT_CPY(&tile_mem_obj_tile[TILE_GHOST_STONE], stoneblackTiles);
	GRIT_CPY(&pal_obj_bank[PAL_GHOST_STONE], stoneblackPal);

	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 |  DCNT_OBJ | DCNT_OBJ_1D;

	REG_BG0CNT= BG_CBB(CBB_BOARD) | BG_SBB(SBB_BOARD) | BG_4BPP | BG_REG_32x32;
	switch (BOARD_SIZE)
	{
		case 3:
			GRIT_CPY(&pal_bg_bank[BGPAL_BOARD], board3Pal);
			GRIT_CPY(&tile_mem[CBB_BOARD], board3Tiles);
			GRIT_CPY(&se_mem[SBB_BOARD], board3Map);
			break;

		case 5:
			GRIT_CPY(&pal_bg_bank[BGPAL_BOARD], board5Pal);
			GRIT_CPY(&tile_mem[CBB_BOARD], board5Tiles);
			GRIT_CPY(&se_mem[SBB_BOARD], board5Map);
			break;
			
		case 7:
			GRIT_CPY(&pal_bg_bank[BGPAL_BOARD], board7Pal);
			GRIT_CPY(&tile_mem[CBB_BOARD], board7Tiles);
			GRIT_CPY(&se_mem[SBB_BOARD], board7Map);
			break;
			
		case 9:
			GRIT_CPY(&pal_bg_bank[BGPAL_BOARD], board9Pal);
			GRIT_CPY(&tile_mem[CBB_BOARD], board9Tiles);
			GRIT_CPY(&se_mem[SBB_BOARD], board9Map);
			break;
			
		case 11:
			GRIT_CPY(&pal_bg_bank[BGPAL_BOARD], board11Pal);
			GRIT_CPY(&tile_mem[CBB_BOARD], board11Tiles);
			GRIT_CPY(&se_mem[SBB_BOARD], board11Map);
			break;
			
		case 13:
			GRIT_CPY(&pal_bg_bank[BGPAL_BOARD], board13Pal);
			GRIT_CPY(&tile_mem[CBB_BOARD], board13Tiles);
			GRIT_CPY(&se_mem[SBB_BOARD], board13Map);
			break;
	}

	REG_BG1CNT= BG_CBB(CBB_BOARDBG) | BG_SBB(SBB_BOARDBG) | BG_4BPP | BG_REG_32x32;
	GRIT_CPY(&tile_mem[CBB_BOARDBG], board_bgTiles);
	GRIT_CPY(&se_mem[SBB_BOARDBG], board_bgMap);

	play_transition_out();

	play_music(MOD_INGAME_SONG);

	oam_init(obj_buffer, OBJ_COUNT);

	obj_set_attr(bee.obj,
		ATTR0_SQUARE,				// Square, regular sprite
		ATTR1_SIZE_32,				// 32*32p,
		ATTR2_PALBANK(PAL_BEE) | TILE_BEE);		// palbank 0, tile 0

	obj_set_attr(ghost_stone,
		ATTR0_SQUARE,
		ATTR1_SIZE_16x16,
		ATTR2_PALBANK(PAL_GHOST_STONE) | TILE_GHOST_STONE);
	obj_hide(ghost_stone);

	//init_stones_sprites();

	restart_game();

	game_state = IN_GAME;

}

void game_loop()
{
	board_bg_vofs--;
	REG_BG1VOFS = board_bg_vofs >> 2;

	switch (mode_1_or_2_players)
	{
		case MODE_1_PLAYER:
			game_loop_1p_vs_cpu();
			break;

		case MODE_2_PLAYERS:
			game_loop_2p();
			break;
	}
}

void game_loop_1p_vs_cpu()
{

	if (key_hit(KEY_START)) {
		init_menu(MENU_PAUSE);
		return;
	}

	if (current_player == PLAYER_1_BLACK)
	{
		evaluate_movement();
		
		if (key_hit(KEY_A)) {
			player_play();
		}
		if (key_hit(KEY_B)) {
			undo_last_move();
		}
	}
	else
	{
		cpu_play();
	}

	update_bee_sprite();

	display_ghost_stone();


}

void game_loop_2p()
{

	if (key_hit(KEY_START)) {
		init_menu(MENU_PAUSE);
		return;
	}
	
	evaluate_movement();

	if (key_hit(KEY_A)) {
		player_play();
	}
	if (key_hit(KEY_B)) {
		undo_last_move();
	}

	update_bee_sprite();

	display_ghost_stone();


}

// TODO delete?
void game_ended_loop()
{
	if (key_hit(KEY_START)) {
		restart_game();
	}
}

void putting_stone_loop() 
{
	board_bg_vofs--;
	REG_BG1VOFS = board_bg_vofs >> 2;

    bee.current_animation = BEE_ATTACK;
	memcpy(&tile_mem_obj_tile[TILE_BEE], &bee32Tiles[(get_sprite_frame_1D(&bee, 20 - putting_stone_delay)) * 8] /*one 4bpp tile = 8 ints*/, bee32TilesLen/16/3);
	display_ghost_stone();
	
	if (--putting_stone_delay == 0)
	{
		obj_hide(ghost_stone);
		bee.current_animation = BEE_IDLE;
		end_turn();
	}
}
