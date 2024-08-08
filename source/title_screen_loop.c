#include "toolbox.h"
#include <tonc.h>
#include "graphics.h"
#include "game_loop.h"
#include "minigame_loop.h"
#include "mem_management.h"
#include "audio.h"
#include "game.h"

#define TITLE_MENU_X		153
#define TITLE_MENU_Y		78
#define TITLE_MENU_SPACE	12

enum TITLE_MENU_CHOICES {
	TITLE_MENU_1_PLAYER,
	TITLE_MENU_2_PLAYERS,
	TITLE_MENU_MINIGAME,
	TITLE_MENU_TUTORIAL,
	TITLE_MENU_OPTIONS,
	TITLE_MENU_NUMBER_OF_CHOICES
};

int titlemenu_selected = TITLE_MENU_1_PLAYER;

int title_bg_hofs = 0;
int title_bg_vofs = 0;
int title_beekeeper_vofs_n = 0;

void init_title_screen()
{
	vid_vsync();
	oam_init(obj_buffer, OBJ_COUNT);

	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_BG3 | DCNT_OBJ | DCNT_OBJ_1D;

	REG_BG3CNT= BG_CBB(CBB_TITLEBG) | BG_SBB(SBB_TITLEBG) | BG_4BPP | BG_REG_32x32;
	GRIT_CPY(&pal_bg_bank[BGPAL_TITLEBG], title_bgPal);
	GRIT_CPY(&tile_mem[CBB_TITLEBG], title_bgTiles);
	GRIT_CPY(&se_mem[SBB_TITLEBG], title_bgMap);

	REG_BG2CNT= BG_CBB(CBB_TITLEBEEKEEPER) | BG_SBB(SBB_TITLEBEEKEEPER) | BG_4BPP | BG_REG_32x32;
	GRIT_CPY(&pal_bg_bank[BGPAL_TITLEBEEKEEPER], title_beekeeperPal);
	GRIT_CPY(&tile_mem[CBB_TITLEBEEKEEPER], title_beekeeperTiles);
	GRIT_CPY(&se_mem[SBB_TITLEBEEKEEPER], title_beekeeperMap);

	REG_BG1CNT= BG_CBB(CBB_TITLEHEXADVANCE) | BG_SBB(SBB_TITLEHEXADVANCE) | BG_4BPP | BG_REG_32x32;
	GRIT_CPY(&pal_bg_bank[BGPAL_TITLEHEXADVANCE], title_hexadvancePal);
	GRIT_CPY(&tile_mem[CBB_TITLEHEXADVANCE], title_hexadvanceTiles);
	GRIT_CPY(&se_mem[SBB_TITLEHEXADVANCE], title_hexadvanceMap);

	REG_BG0CNT= BG_CBB(CBB_TITLEMENUTEXT) | BG_SBB(SBB_TITLEMENUTEXT) | BG_4BPP | BG_REG_32x32;
	GRIT_CPY(&pal_bg_bank[BGPAL_TITLEMENUTEXT], title_menutextPal);
	GRIT_CPY(&tile_mem[CBB_TITLEMENUTEXT], title_menutextTiles);
	GRIT_CPY(&se_mem[SBB_TITLEMENUTEXT], title_menutextMap);


	title_bg_hofs = 0;
	title_bg_vofs = 0;

	REG_BG0HOFS = 0;
	REG_BG1HOFS = 0;
	REG_BG2HOFS = 0;
	REG_BG3HOFS = 0;
	REG_BG0VOFS = 0;
	REG_BG1VOFS = 0;
	REG_BG2VOFS = 0;
	REG_BG3VOFS = 0;

	title_beekeeper_vofs_n = 0;

	GRIT_CPY(&tile_mem_obj_tile[TILE_CURSOR], menu_cursorTiles);
	GRIT_CPY(&pal_obj_bank[PAL_MENUS], menu_palettePal);

	obj_set_attr(&obj_buffer[OAM_CURSOR], 
		ATTR0_WIDE,
		ATTR1_SIZE_8x16,
		ATTR2_PALBANK(PAL_MENUS) | TILE_CURSOR);
	
	// titlemenu_selected = TITLE_MENU_1_PLAYER;
	obj_set_pos(&obj_buffer[OAM_CURSOR], TITLE_MENU_X, TITLE_MENU_Y);

	game_state = TITLE_SCREEN;

	play_music( MOD_TITLESCREEN);

}


void title_screen_loop()
{
	// useless modulo (power of 2)
	title_bg_hofs = mod(title_bg_hofs-1, 256);
	title_bg_vofs = mod(title_bg_vofs+1, 256);
	REG_BG3HOFS = title_bg_hofs;
	REG_BG3VOFS = title_bg_vofs;

	// useless modulo (power of 2)
	title_beekeeper_vofs_n = mod(title_beekeeper_vofs_n+1, 0xFFFF);

	// func input is [0, 0xFFFF] for [0,2π[
	// func output >> 12 is between 0 and 1 (it's a .12f)
	REG_BG2VOFS = lu_cos(title_beekeeper_vofs_n *500) >> 10; 

	
	if (key_hit(KEY_UP))
	{
		titlemenu_selected = mod((titlemenu_selected - 1), TITLE_MENU_NUMBER_OF_CHOICES);
		mmEffectEx(&sfx_cursor);
	}
	if (key_hit(KEY_DOWN))
	{
		titlemenu_selected = mod((titlemenu_selected + 1), TITLE_MENU_NUMBER_OF_CHOICES);
		mmEffectEx(&sfx_cursor);
	}

	int offset = 1 * ((global_frame>>4)&1);

	obj_set_pos(&obj_buffer[OAM_CURSOR], TITLE_MENU_X - offset, TITLE_MENU_Y + TITLE_MENU_SPACE * titlemenu_selected);
	
	if (key_hit(KEY_A))
	{
        mmEffectEx(&sfx_confirm);
		switch (titlemenu_selected)
		{
			case TITLE_MENU_1_PLAYER:
				play_music(MOD_INGAME_SONG);
				mode_1_or_2_players = MODE_1_PLAYER;
				init_game_loop();
				return;
			case TITLE_MENU_2_PLAYERS:
				play_music(MOD_INGAME_SONG);
				mode_1_or_2_players = MODE_2_PLAYERS;
				init_game_loop();
				return;
			case TITLE_MENU_MINIGAME:
				play_music(MOD_INGAME_SONG);
				init_minigame_loop();
				return;
		}
	}

	// TODO real options menu
	if (titlemenu_selected == TITLE_MENU_OPTIONS)
	{
		if (key_hit(KEY_L) && BOARD_SIZE > 3)
		{
			mmEffectEx(&sfx_cursor);
			BOARD_SIZE -= 2;
		}
		if (key_hit(KEY_R) && BOARD_SIZE < 13)
		{
			mmEffectEx(&sfx_cursor);
			BOARD_SIZE += 2;
		}
	}
}
