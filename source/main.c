#include <tonc.h>
#include <string.h>
#include <stdio.h>

#include "coordinates.h"
#include "movement.h"
#include "toolbox.h"
#include "PlayerSprite.h"
#include "graphics.h"
#include "game.h"

int global_frame = 0;
int current_player = PLAYER_1_BLACK;
OBJ_ATTR obj_buffer[OBJ_COUNT];
GAME_STATE game_state = IN_GAME;

void init_game()
{
	memcpy(&tile_mem[4][0], bee32Tiles, bee32TilesLen/3/16); // on ne prend qu'un sprite dans la sheet de 16 sprites * 3 animations
	memcpy(pal_obj_mem, bee32Pal, bee32PalLen);
	memcpy(&tile_mem[4][64], stoneblackTiles, stoneblackTilesLen);
	memcpy(&pal_obj_bank[1], stoneblackPal, stoneblackPalLen);

	oam_init(obj_buffer, OBJ_COUNT);

	// Load palette
	memcpy(pal_bg_mem, hexPal, hexPalLen);
	// Load tiles into CBB 0
	memcpy(&tile_mem[0][0], hexTiles, hexTilesLen);
	// Load map into SBB 30
	memcpy(&se_mem[30][0], hexMap, hexMapLen);

	REG_BG0CNT= BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x32;
	REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;


	obj_set_attr(bee.obj,
		ATTR0_SQUARE,				// Square, regular sprite
		ATTR1_SIZE_32,				// 32*32p,
		ATTR2_PALBANK(0) | 0);		// palbank 0, tile 0

	obj_set_attr(ghost_stone,
		ATTR0_SQUARE,
		ATTR1_SIZE_16x16,
		ATTR2_PALBANK(1) | 64);

	init_stones_sprites();

}

void game_loop()
{

	if (key_hit(KEY_A)) {
		play();
	}

	if (key_hit(KEY_START)) {
		restart_game();
	}
	
	evaluate_movement();

	update_bee_sprite();

	display_ghost_stone();


}

void game_ended_loop()
{
	if (key_hit(KEY_START)) {
		restart_game();
	}
}

void putting_stone_loop() 
{
    bee.current_animation = BEE_ATTACK;
	memcpy(&tile_mem[4][0], &bee32Tiles[(get_sprite_frame_1D(&bee, 20 - putting_stone_delay)) * 8] /*one 4bpp tile = 8 ints*/, bee32TilesLen/16/3);
	display_ghost_stone();
	
	if (--putting_stone_delay == 0)
	{
		end_turn();
	}
}

int main()
{

	init_game();

	while(1)
	{

		vid_vsync();		
	
		global_frame++;

		key_poll();

		switch (game_state)
		{
			case IN_GAME:
				game_loop();
				break;

			case GAME_ENDED:
				game_ended_loop();
				break;
			
			case PUTTING_STONE:
				putting_stone_loop();
				break;
		}

		obj_copy(obj_mem, obj_buffer, OBJ_COUNT);

	}

	return 0;
}
