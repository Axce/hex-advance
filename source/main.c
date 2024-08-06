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


int global_frame = 0;
int current_player = PLAYER_1_BLACK;
OBJ_ATTR obj_buffer[OBJ_COUNT];
GAME_STATE game_state;

int main()
{
	init_audio();

	play_music(MOD_TITLESCREEN);

	init_title_screen();

	while(1)
	{

		key_poll();

		vid_vsync();
		mmFrame();
	
		global_frame++;

		key_poll();

		switch (game_state)
		{
			case TITLE_SCREEN:
				title_screen_loop();
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
