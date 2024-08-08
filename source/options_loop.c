#include <tonc.h>
#include "toolbox.h"
#include "mem_management.h"
#include "title_screen_loop.h"
#include "graphics.h"
#include "game.h"
#include "audio.h"

#define OPTIONS_LINE_SPACE  24

enum OPTIONS_LINES
{
    OPTL_FIRSTMOVE,
    OPTL_BOARDSIZE,
    OPTIONS_NUMBER_OF_LINES
};

int options_line_selected = 0;
int first_move = 0;

int opt_cursor_x;
int opt_cursor_y;

void init_options_loop()
{
	vid_vsync();
	oam_init(obj_buffer, OBJ_COUNT);

	REG_DISPCNT= DCNT_MODE0 /*| DCNT_BG0*/ | DCNT_BG1 | DCNT_BG2 | DCNT_BG3 | DCNT_OBJ | DCNT_OBJ_1D;


	REG_BG2CNT= BG_CBB(CBB_OPTIONS) | BG_SBB(SBB_OPTIONS) | BG_4BPP | BG_REG_32x32;
	GRIT_CPY(&pal_bg_bank[BGPAL_OPTIONS], menu_optionsPal);
	GRIT_CPY(&tile_mem[CBB_OPTIONS], menu_optionsTiles);
	GRIT_CPY(&se_mem[SBB_OPTIONS], menu_optionsMap);

	REG_BG2VOFS = 0;

    opt_cursor_x = 0;
    opt_cursor_y = 109;
    
	GRIT_CPY(&tile_mem_obj_tile[TILE_CURSOR], menu_cursorTiles);
	GRIT_CPY(&pal_obj_bank[PAL_MENUS], menu_palettePal);
	obj_set_attr(&obj_buffer[OAM_CURSOR],
		ATTR0_WIDE,
		ATTR1_SIZE_8x16,
		ATTR2_PALBANK(PAL_MENUS) | TILE_CURSOR);
    obj_set_pos(&obj_buffer[OAM_CURSOR_BS], opt_cursor_x, opt_cursor_y);

    GRIT_CPY(&tile_mem_obj_tile[TILE_CURSOR_BS], menu_cursor_bsTiles);
	obj_set_attr(&obj_buffer[OAM_CURSOR_BS],
		ATTR0_SQUARE,
		ATTR1_SIZE_8x8,
		ATTR2_PALBANK(PAL_MENUS) | TILE_CURSOR_BS);
    obj_set_pos(&obj_buffer[OAM_CURSOR_BS], 99, 143);

    GRIT_CPY(&tile_mem_obj_tile[TILE_CURSOR_FM], menu_cursor_fmTiles);
	obj_set_attr(&obj_buffer[OAM_CURSOR_FM],
		ATTR0_WIDE,
		ATTR1_SIZE_32x8,
		ATTR2_PALBANK(PAL_MENUS) | TILE_CURSOR_FM);
    obj_set_pos(&obj_buffer[OAM_CURSOR_FM], 87, 119);

	game_state = OPTIONS;


}

void options_loop()
{
	// useless modulo (power of 2)
	title_bg_hofs = mod(title_bg_hofs-1, 256);
	title_bg_vofs = mod(title_bg_vofs+1, 256);
	REG_BG3HOFS = title_bg_hofs;
	REG_BG3VOFS = title_bg_vofs;
	
	if (key_hit(KEY_UP))
	{
		options_line_selected = mod((options_line_selected - 1), OPTIONS_NUMBER_OF_LINES);
		mmEffectEx(&sfx_cursor);
	}
	if (key_hit(KEY_DOWN))
	{
		options_line_selected = mod((options_line_selected + 1), OPTIONS_NUMBER_OF_LINES);
		mmEffectEx(&sfx_cursor);
	}

	int offset = 1 * ((global_frame>>4)&1);

	obj_set_pos(&obj_buffer[OAM_CURSOR], opt_cursor_x - offset, opt_cursor_y + OPTIONS_LINE_SPACE * options_line_selected);
	
	// if (key_hit(KEY_A))
	// {
    //     mmEffectEx(&sfx_confirm);
	// 	switch (titlemenu_selected)
	// 	{
	// 		case TITLE_MENU_1_PLAYER:
	// 			play_music(MOD_INGAME_SONG);
	// 			mode_1_or_2_players = MODE_1_PLAYER;
	// 			init_game_loop();
	// 			return;
	// 		case TITLE_MENU_2_PLAYERS:
	// 			play_music(MOD_INGAME_SONG);
	// 			mode_1_or_2_players = MODE_2_PLAYERS;
	// 			init_game_loop();
	// 			return;
	// 		case TITLE_MENU_MINIGAME:
	// 			play_music(MOD_INGAME_SONG);
	// 			init_minigame_loop();
	// 			return;
	// 	}
	// }

	// // TODO real options menu
	// if (titlemenu_selected == TITLE_MENU_OPTIONS)
	// {
	// 	if (key_hit(KEY_L) && BOARD_SIZE > 3)
	// 	{
	// 		mmEffectEx(&sfx_cursor);
	// 		BOARD_SIZE -= 2;
	// 	}
	// 	if (key_hit(KEY_R) && BOARD_SIZE < 13)
	// 	{
	// 		mmEffectEx(&sfx_cursor);
	// 		BOARD_SIZE += 2;
	// 	}
	// }

}