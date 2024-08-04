#include "toolbox.h"
#include <tonc.h>
#include "graphics.h"
#include "game_loop.h"
#include "minigame_loop.h"

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

int selected;

void init_title_screen()
{
	vid_vsync();
	oam_init(obj_buffer, OBJ_COUNT);
	memset32(obj_mem, 0, sizeof(OBJ_ATTR)*1024);
	REG_DISPCNT= DCNT_MODE3 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
	GRIT_CPY(vid_mem, title_screenBitmap);

	GRIT_CPY(&tile_mem_obj[1][0], menu_cursorTiles);
	GRIT_CPY(&pal_obj_bank[0], menu_cursorPal);

	obj_set_attr(&obj_buffer[0], 
		ATTR0_WIDE,
		ATTR1_SIZE_8x16,
		ATTR2_PALBANK(0) | 512);
	
	selected = TITLE_MENU_1_PLAYER;
	obj_set_pos(&obj_buffer[0], TITLE_MENU_X, TITLE_MENU_Y);

	game_state = TITLE_SCREEN;

}

void title_screen_loop()
{
	if (key_hit(KEY_UP))
		selected = mod((selected - 1), TITLE_MENU_NUMBER_OF_CHOICES);
	if (key_hit(KEY_DOWN))
		selected = mod((selected + 1), TITLE_MENU_NUMBER_OF_CHOICES);

	int offset = 1 * ((global_frame>>4)&1);

	obj_set_pos(&obj_buffer[0], TITLE_MENU_X - offset, TITLE_MENU_Y + 12 * selected);
	
	if (key_hit(KEY_A))
	{
		switch (selected)
		{
			case TITLE_MENU_1_PLAYER:
				mode_1_or_2_players = MODE_1_PLAYER;
				init_game_loop();
				return;
			case TITLE_MENU_2_PLAYERS:
				mode_1_or_2_players = MODE_2_PLAYERS;
				init_game_loop();
				return;
			case TITLE_MENU_MINIGAME:
				init_minigame_loop();
				return;
		}
	}
}
