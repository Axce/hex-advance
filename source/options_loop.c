#include <tonc.h>
#include "toolbox.h"
#include "mem_management.h"
#include "title_screen_loop.h"
#include "graphics.h"
#include "game.h"
#include "audio.h"

#define OPTIONS_LINE_SPACE  16

#define OPTL_FIRSTMOVE_Y	103
#define OPTL_BOARDSIZE_Y	OPTL_FIRSTMOVE_Y + OPTIONS_LINE_SPACE
#define OPTL_MUSIC_Y		OPTL_BOARDSIZE_Y + OPTIONS_LINE_SPACE
#define OPTL_SOUND_Y		OPTL_MUSIC_Y + OPTIONS_LINE_SPACE


enum OPTIONS_LINES
{
    OPTL_FIRSTMOVE,
    OPTL_BOARDSIZE,
	OPTL_MUSIC,
	OPTL_SOUND,
    OPTIONS_NUMBER_OF_LINES
};

int options_line_selected = 0;

int option_first_move;
bool option_music;
bool option_sound;

const int opt_fm_cursor_positions[3] = {88, 148, 200};

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
    opt_cursor_y = 93;
    
	GRIT_CPY(&tile_mem_obj_tile[TILE_CURSOR], menu_cursorTiles);
	GRIT_CPY(&pal_obj_bank[PAL_MENUS], menu_palettePal);
	obj_set_attr(&obj_buffer[OAM_CURSOR],
		ATTR0_WIDE,
		ATTR1_SIZE_8x16,
		ATTR2_PALBANK(PAL_MENUS) | TILE_CURSOR);
    obj_set_pos(&obj_buffer[OAM_CURSOR], opt_cursor_x, opt_cursor_y);

    GRIT_CPY(&tile_mem_obj_tile[TILE_CURSOR_FM], menu_cursor_fmTiles);
	obj_set_attr(&obj_buffer[OAM_CURSOR_FM],
		ATTR0_WIDE,
		ATTR1_SIZE_32x8,
		ATTR2_PALBANK(PAL_MENUS) | TILE_CURSOR_FM);
    obj_set_pos(&obj_buffer[OAM_CURSOR_FM], opt_fm_cursor_positions[option_first_move], OPTL_FIRSTMOVE_Y);
    
	GRIT_CPY(&tile_mem_obj_tile[TILE_CURSOR_BS], menu_cursor_bsTiles);
	obj_set_attr(&obj_buffer[OAM_CURSOR_BS],
		ATTR0_SQUARE,
		ATTR1_SIZE_8x8,
		ATTR2_PALBANK(PAL_MENUS) | TILE_CURSOR_BS);
    obj_set_pos(&obj_buffer[OAM_CURSOR_BS], 69 + 10*BOARD_SIZE, OPTL_BOARDSIZE_Y);

	obj_set_attr(&obj_buffer[OAM_CURSOR_MUSIC],
		ATTR0_SQUARE,
		ATTR1_SIZE_8x8,
		ATTR2_PALBANK(PAL_MENUS) | TILE_CURSOR_BS);
    obj_set_pos(&obj_buffer[OAM_CURSOR_MUSIC], 123 + 48*(!option_music), OPTL_MUSIC_Y);
	
	obj_set_attr(&obj_buffer[OAM_CURSOR_SOUND],
		ATTR0_SQUARE,
		ATTR1_SIZE_8x8,
		ATTR2_PALBANK(PAL_MENUS) | TILE_CURSOR_BS);
    obj_set_pos(&obj_buffer[OAM_CURSOR_SOUND], 123 + 48*(!option_sound), OPTL_SOUND_Y);

	game_state = OPTIONS;


}

void options_loop()
{
	// // useless modulo (power of 2)
	title_bg_hofs = mod(title_bg_hofs-1, 256);
	title_bg_vofs = mod(title_bg_vofs+1, 256);
	REG_BG3HOFS = title_bg_hofs;
	REG_BG3VOFS = title_bg_vofs;
	
	if (key_hit(KEY_UP))
	{
		options_line_selected = mod((options_line_selected - 1), OPTIONS_NUMBER_OF_LINES);
		play_sfx(&sfx_cursor);
	}
	if (key_hit(KEY_DOWN))
	{
		options_line_selected = mod((options_line_selected + 1), OPTIONS_NUMBER_OF_LINES);
		play_sfx(&sfx_cursor);
	}

	int offset = 1 * ((global_frame>>4)&1);

	obj_set_pos(&obj_buffer[OAM_CURSOR], opt_cursor_x - offset, opt_cursor_y + OPTIONS_LINE_SPACE * options_line_selected);
	
    if (key_hit(KEY_B))
    {
        play_sfx(&sfx_cursor);
		// save to SRAM
		sram_mem[SRAM_BOARDSIZE] = BOARD_SIZE;
		sram_mem[SRAM_FIRSTMOVE] = option_first_move;
		sram_mem[SRAM_MUSIC] = option_music;
		sram_mem[SRAM_SOUND] = option_sound;
        init_title_screen();
        return;
    }

    switch (options_line_selected)
    {
        case OPTL_FIRSTMOVE:
            if (key_hit(KEY_LEFT) && option_first_move > 0)
            {
                play_sfx(&sfx_cursor);
                option_first_move--;
            }
            if (key_hit(KEY_RIGHT) && option_first_move < 2)
            {
                play_sfx(&sfx_cursor);
                option_first_move++;
            }
            obj_set_pos(&obj_buffer[OAM_CURSOR_FM], opt_fm_cursor_positions[option_first_move], OPTL_FIRSTMOVE_Y);
            break;

        case OPTL_BOARDSIZE:
            if (key_hit(KEY_LEFT) && BOARD_SIZE > 3)
            {
                play_sfx(&sfx_cursor);
                BOARD_SIZE -= 2;
            }
            if (key_hit(KEY_RIGHT) && BOARD_SIZE < 13)
            {
                play_sfx(&sfx_cursor);
                BOARD_SIZE += 2;
            }
            obj_set_pos(&obj_buffer[OAM_CURSOR_BS], 69 + 10*BOARD_SIZE, OPTL_BOARDSIZE_Y);
            break;

        case OPTL_MUSIC:
            if (key_hit(KEY_RIGHT) && option_music)
            {
                play_sfx(&sfx_cursor);
                option_music = 0;
                mmStop();
            }
            if (key_hit(KEY_LEFT) && !option_music)
            {
                play_sfx(&sfx_cursor);
                option_music = 1;
                play_music(MOD_TITLESCREEN);
            }
            obj_set_pos(&obj_buffer[OAM_CURSOR_MUSIC], 123 + 48*(!option_music), OPTL_MUSIC_Y);
            break;

        case OPTL_SOUND:
            if (key_hit(KEY_RIGHT) && option_sound)
            {
                option_sound = 0;
            }
            if (key_hit(KEY_LEFT) && !option_sound)
            {
                option_sound = 1;
                play_sfx(&sfx_cursor);

            }
            obj_set_pos(&obj_buffer[OAM_CURSOR_SOUND], 123 + 48*(!option_sound), OPTL_SOUND_Y);
            break;
    }

}