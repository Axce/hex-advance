#include <tonc.h>
#include <string.h>
#include <stdio.h>

#include "coordinates.h"
#include "movement.h"
#include "toolbox.h"
#include "PlayerSprite.h"
#include "graphics.h"
#include "game.h"
#include "title_screen_loop.h"
#include "game_loop.h"
#include "minigame_loop.h"
#include "menus.h"
#include "audio.h"
#include "cpu_player.h"
#include "options_loop.h"
#include "mem_management.h"

int global_frame = 0;
int current_player = PLAYER_1_BLACK;
OBJ_ATTR obj_buffer[OBJ_COUNT];
GAME_STATE game_state = TITLE_SCREEN;

void onVBlank()
{
	mmVBlank();
	
	global_frame++;

	qran();

	key_poll();

	if (thinking_progress)
	{
        update_bee_thinking_position();
        update_bee_sprite();
		obj_copy(obj_mem, obj_buffer, OBJ_COUNT);
		board_bg_vofs--;
		REG_BG1VOFS = board_bg_vofs >> 2;
	}

	mmFrame();
}

int main()
{

	// init SRAM
	if (sram_mem[SRAM_BOARDSIZE] == 0xFF)
		sram_mem[SRAM_BOARDSIZE] = 11;
	if (sram_mem[SRAM_FIRSTMOVE] == 0xFF)
		sram_mem[SRAM_FIRSTMOVE] = 0;
	if (sram_mem[SRAM_MUSIC] == 0xFF)
		sram_mem[SRAM_MUSIC] = 1;
	if (sram_mem[SRAM_SOUND] == 0xFF)
		sram_mem[SRAM_SOUND] = 1;

	// init game from SRAM
	BOARD_SIZE = sram_mem[SRAM_BOARDSIZE];
	option_first_move = sram_mem[SRAM_FIRSTMOVE];
	option_music = sram_mem[SRAM_MUSIC];
	option_sound = sram_mem[SRAM_SOUND];
	init_player_spawns();
	
	sqran(1312);

	irq_init(NULL);
	//irq_add(II_VBLANK, mmVBlank);
	irq_add(II_VBLANK, onVBlank);
    irq_enable(II_VBLANK);

	init_audio();

	
	// splash screen

	GRIT_CPY(vid_mem, gbajamBitmap);
	GRIT_CPY(pal_bg_mem, gbajamPal);

	REG_DISPCNT = DCNT_MODE4 | DCNT_BG2;

	int timer = 180;
	while (timer--)
	{
		VBlankIntrWait();
		if (key_hit(KEY_FULL))
		{
			break;
		}
	}

	// game

	play_music(MOD_TITLESCREEN);

	init_title_screen();





	while(1)
	{

		VBlankIntrWait();

		switch (game_state)
		{
			case TITLE_SCREEN:
				title_screen_loop();
				break;

			case TUTORIAL:
				tutorial_loop();
				break;

			case OPTIONS:
				options_loop();
				break;

			case IN_GAME:
				game_loop();
				break;

			case GAME_ENDED:
				game_ended_loop();
				break;
			
			case PUTTING_STONE:
				putting_stone_loop();
				break;

			case MINIGAME:
				minigame_loop();
				break;

			case MINIGAME_PUTTING_STONE:
				minigame_putting_stone_loop();
				break;

			case MINIGAME_ENDED:
				minigame_ended_loop();
				break;

			case MENUING:
				menu_loop();
				break;
		}

		obj_copy(obj_mem, obj_buffer, OBJ_COUNT);

	}

	return 0;
}
