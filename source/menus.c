#include <tonc.h>
#include "menus.h"
#include "toolbox.h"
#include "graphics.h"
#include "mem_management.h"
#include "title_screen_loop.h"
#include "game_loop.h"
#include "minigame_loop.h"
#include "audio.h"

#define MENU_POS_X  SCREEN_WIDTH/2 - 32
#define MENU_POS_Y  SCREEN_HEIGHT/2 - 32

#define CURSOR_BASE_X  SCREEN_WIDTH/2 - 33
#define CURSOR_BASE_Y  SCREEN_HEIGHT/2 + 1

#define ENTRIES_SPACE    12

#define PAUSE_MENU_NUMBER_OF_CHOICES    3
#define RESTART_MENU_NUMBER_OF_CHOICES  2

MENU_TYPE current_menu_type;
int selected = 0;
bool was_in_minigame = false;

void init_menu(MENU_TYPE type)
{
    play_sfx(&sfx_cursor);
    current_menu_type = type;
    selected = 0;
    
    was_in_minigame = (game_state == MINIGAME);
    game_state = MENUING;
    
    int cursor_y = CURSOR_BASE_Y;

    GRIT_CPY(&pal_obj_bank[PAL_MENUS], menu_palettePal);

    switch (type)
    {
        case MENU_PAUSE:
            GRIT_CPY(&tile_mem_obj_tile[TILE_MENUS], menu_pauseTiles);
            cursor_y -= ENTRIES_SPACE;
            break;
        case MENU_P1WINS:
            GRIT_CPY(&tile_mem_obj_tile[TILE_MENUS], menu_p1winsTiles);
            break;
        case MENU_P2WINS:
            GRIT_CPY(&tile_mem_obj_tile[TILE_MENUS], menu_p2winsTiles);
            break;
        case MENU_YOUWIN:
            GRIT_CPY(&tile_mem_obj_tile[TILE_MENUS], menu_youwinTiles);
            break;
        case MENU_GAMEOVER:
            GRIT_CPY(&tile_mem_obj_tile[TILE_MENUS], menu_gameoverTiles);
            break;
        default:
            while(1);
    }

    obj_set_attr(&obj_buffer[OAM_MENUS],
        ATTR0_SQUARE,
        ATTR1_SIZE_64x64,
        ATTR2_PALBANK(PAL_MENUS) | ATTR2_ID(TILE_MENUS));
    obj_set_pos(&obj_buffer[OAM_MENUS], MENU_POS_X, MENU_POS_Y);

	obj_set_attr(&obj_buffer[OAM_CURSOR], 
		ATTR0_WIDE,
		ATTR1_SIZE_8x16,
		ATTR2_PALBANK(PAL_MENUS) | ATTR2_ID(TILE_CURSOR));
    obj_set_pos(&obj_buffer[OAM_CURSOR], CURSOR_BASE_X, cursor_y);


}

void menu_loop()
{
    switch (current_menu_type)
    {
        case MENU_PAUSE:
            menu_pause();
            break;

        case MENU_P1WINS:
        case MENU_P2WINS:
        case MENU_YOUWIN:
        case MENU_GAMEOVER:
            menu_restart();
            break;
    }

}

void menu_pause()
{
	if (key_hit(KEY_UP))
    {

		selected = mod((selected - 1), PAUSE_MENU_NUMBER_OF_CHOICES);
        play_sfx(&sfx_cursor);
    }
	if (key_hit(KEY_DOWN))
	{

    	selected = mod((selected + 1), PAUSE_MENU_NUMBER_OF_CHOICES);
        play_sfx(&sfx_cursor);
    }

	int offset = 1 * ((global_frame>>4)&1);

	obj_set_pos(&obj_buffer[OAM_CURSOR], CURSOR_BASE_X - offset, CURSOR_BASE_Y + ENTRIES_SPACE * (selected - 1)); // selected-1 is because pause menu displays one entry higher
	
	if (key_hit(KEY_A))
	{
        play_sfx(&sfx_confirm);
		switch (selected)
		{
			case 0: // Continue
                close_menu();
                if (was_in_minigame)
                    game_state = MINIGAME;
                else
                    game_state = IN_GAME;
				break;
			case 1: // Restart
                if (was_in_minigame)
                    init_minigame_loop();
                else
				    init_game_loop();
				break;
			case 2: // Quit
            	play_music(MOD_TITLESCREEN);
				init_title_screen();
				break;
		}
	}

    if (key_hit(KEY_START))
    {
        close_menu();
        if (was_in_minigame)
            game_state = MINIGAME;
        else
            game_state = IN_GAME;
    }

}

void menu_restart()
{
	if (key_hit(KEY_UP))
    {
		selected = mod((selected - 1), RESTART_MENU_NUMBER_OF_CHOICES);
        play_sfx(&sfx_cursor);
    }
	if (key_hit(KEY_DOWN))
    {
		selected = mod((selected + 1), RESTART_MENU_NUMBER_OF_CHOICES);
        play_sfx(&sfx_cursor);
    }

	int offset = 1 * ((global_frame>>4)&1);

	obj_set_pos(&obj_buffer[OAM_CURSOR], CURSOR_BASE_X - offset, CURSOR_BASE_Y + ENTRIES_SPACE * (selected));
	
	if (key_hit(KEY_A))
	{
        play_sfx(&sfx_confirm);
		switch (selected)
		{
			case 0: // Restart
                if (was_in_minigame)
                    init_minigame_loop();
                else
				    init_game_loop();
				break;
			case 1: // Quit
            	play_music(MOD_TITLESCREEN);
				init_title_screen();
				break;
		}
	}
}

void close_menu()
{
    obj_hide(&obj_buffer[OAM_MENUS]); 
    obj_hide(&obj_buffer[OAM_CURSOR]); 
}