#include <tonc.h>
#include <string.h>

#include "bee.h"
#include "beedark.h"
#include "hex.h"
#include "stonewhite.h"
#include "stoneblack.h"

#define OBJ_COUNT 128
#define PLAYER_CENTER_X 87
#define PLAYER_CENTER_Y 40

OBJ_ATTR obj_buffer[OBJ_COUNT];

enum ORIENTATIONS {
	NORTH,
	WEST,
	EAST,
	SOUTH
};

enum PLAYERS {
	PLAYER_1,	// yellow bee, black stones
	PLAYER_2	// red bee, white stones
};

typedef struct {
	OBJ_ATTR* obj;
	int width, height;	//in tiles
	int anim_delay;		//in frames
	int anim_frames;
	int orientation;
	int x,y;
	int max_speed;
	int x_speed, y_speed;
} PlayerSprite;

// in tiles
int get_sprite_frame_2D(PlayerSprite* sprite, int global_frame) {
	int sprite_frame = sprite->width * ((global_frame/sprite->anim_delay) % sprite->anim_frames);
	int orientation_offset = (sprite->width * sprite->height * sprite->anim_frames * sprite->orientation);
	return sprite_frame + orientation_offset;
}

// in tiles
int get_sprite_frame_1D(PlayerSprite* sprite, int global_frame) {
	int sprite_frame = sprite->width * sprite->height * ((global_frame/sprite->anim_delay) % sprite->anim_frames);
	int orientation_offset = (sprite->width * sprite->height * sprite->anim_frames * sprite->orientation);
	return sprite_frame + orientation_offset;
}

int closer_to_zero(int n) {
	if (n)
		n += (n>0) ? (-1) : 1;
	return n;
}
int cap_speed_and_apply_friction(int n, int accel_friction, int max_speed) {
	
	return clamp(n, -max_speed, max_speed+1);
	/*
	if (n==0) return 0;
	if (accel_friction==0) return ;

	n = clamp(n, (-accel_friction)*max_speed, accel_friction*max_speed+1);
	
	n += (n<0) ? (-accel_friction) : accel_friction;
	
	return n/accel_friction;*/
}



int global_frame = 0;
int player = PLAYER_1;

PlayerSprite bee = {
	&obj_buffer[0],
	8, 8,
	5,
	4,
	SOUTH,
	PLAYER_CENTER_X, PLAYER_CENTER_Y,
	2, // vitesse max
	0, 0
};

OBJ_ATTR* ghost_stone = &obj_buffer[1];


void get_stone_hex_coor(int x, int y) {
	
}

void switch_player() {

	player ^= 1;

	bee.x = PLAYER_CENTER_X;
	bee.y = PLAYER_CENTER_Y;
	bee.orientation = SOUTH;

	if (player == PLAYER_1) {
		memcpy(pal_obj_mem, beePal, beePalLen);
		memcpy(&tile_mem[4][64], stonewhiteTiles, stonewhiteTilesLen);
		memcpy(&pal_obj_bank[1], stonewhitePal, stonewhitePalLen);
	}
	else {
		memcpy(pal_obj_mem, beedarkPal, beedarkPalLen);
		memcpy(&tile_mem[4][64], stoneblackTiles, stoneblackTilesLen);
		memcpy(&pal_obj_bank[1], stoneblackPal, stoneblackPalLen);
	}
}

void evaluate_movement() {
	
	int accel_friction = 2; // 1 = instantané ; 2 = une frame mid-speed, etc.

	int delta_speed = bee.max_speed / accel_friction;

	if(key_is_down(KEY_UP)) {
		bee.orientation = NORTH;
		bee.y_speed-= delta_speed;
	}
	else if(key_is_down(KEY_DOWN)) {
		bee.orientation = SOUTH;
		bee.y_speed+= delta_speed;
	}
	else {
		bee.y_speed = closer_to_zero(bee.y_speed);
	}

	if(key_is_down(KEY_LEFT)) {
		bee.orientation = WEST;
		bee.x_speed-= delta_speed;
	}
	else if(key_is_down(KEY_RIGHT)) {
		bee.orientation = EAST;
		bee.x_speed+= delta_speed;
	}
	else {
		bee.x_speed = closer_to_zero(bee.x_speed);
	}

	bee.x_speed = cap_speed_and_apply_friction(bee.x_speed, accel_friction, bee.max_speed);
 	bee.y_speed = cap_speed_and_apply_friction(bee.y_speed, accel_friction, bee.max_speed);

	bee.x += bee.x_speed;
	bee.y += bee.y_speed;
	
	/*
	bee.x += key_tri_horz();
	bee.y += key_tri_vert();
	*/
}

void display_ghost_stone() {
	int ghost_x = (bee.x+37)/10*10 - 8;  //adjust de +32px (moitié de largeur de bee) et -8px (moitié de largeur de ghost), et puis fine-tuning
	int ghost_y;
	if ((ghost_x/10) & 1)
		ghost_y = (bee.y+32)/12*12 - 8 + 6 + 3;
	else
		ghost_y = (bee.y+32+6)/12*12 - 8 + 3;

	obj_set_pos(ghost_stone, ghost_x, ghost_y);
	ghost_stone->attr0 ^= ATTR0_HIDE;
}

void update_sprites() {
	//bee.obj->attr2 = ATTR2_PALBANK(0) | get_sprite_frame_2D(&bee, global_frame);
	memcpy(&tile_mem[4][0], beeTiles + (get_sprite_frame_1D(&bee, global_frame)) * 8 /*one 4bpp tile = 8 ints*/, beeTilesLen/16);
	obj_set_pos(bee.obj, bee.x, bee.y);
}

int main()
{

	memcpy(&tile_mem[4][0], beeTiles, beeTilesLen/16); // on ne prend qu'un sprite dans la sheet de 16
	memcpy(pal_obj_mem, beePal, beePalLen);
	memcpy(&tile_mem[4][64], stonewhiteTiles, stonewhiteTilesLen);
	memcpy(&pal_obj_bank[1], stonewhitePal, stonewhitePalLen);

	

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
		ATTR1_SIZE_64,				// 64x64p,
		ATTR2_PALBANK(0) | 0);		// palbank 0, tile 0

	obj_set_attr(ghost_stone,
		ATTR0_SQUARE,
		ATTR1_SIZE_16x16,
		ATTR2_PALBANK(1) | 64);

	while(1)
	{

		vid_vsync();		
		
		global_frame++;

		key_poll();

		if (key_hit(KEY_A)) {
			switch_player();
		}
		
		evaluate_movement();

		display_ghost_stone();
		
		update_sprites();

		obj_copy(obj_mem, obj_buffer, OBJ_COUNT);
	}

	return 0;
}
