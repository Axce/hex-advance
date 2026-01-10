#include <tonc.h>

#include "movement.h"
#include "toolbox.h"
#include "PlayerSprite.h"
#include "audio.h"

enum BEE_ANIMATIONS prev_mvt = BEE_IDLE;

int closer_to_zero(int n) {
	if (n)
		n += (n>0) ? (-1) : 1;
	return n;
}
int cap_speed_and_apply_friction(int n, int accel_friction, int max_speed) {
	
	//remove unused warning
	(void) accel_friction;

	return clamp(n, -max_speed, max_speed+1);
	/*
	if (n==0) return 0;
	if (accel_friction==0) return ;

	n = clamp(n, (-accel_friction)*max_speed, accel_friction*max_speed+1);
	
	n += (n<0) ? (-accel_friction) : accel_friction;
	
	return n/accel_friction;*/
}

void evaluate_movement() {
	
	int accel_friction = 2; // 1 = instantané ; 2 = une frame mid-speed, etc.

	int delta_speed = bee.max_speed / accel_friction;

	bee.current_animation = BEE_IDLE;

	if(key_is_down(KEY_UP)) {
		bee.orientation = NORTH;
		bee.current_animation = BEE_FLYING;
		bee.y_speed-= delta_speed;
	}
	else if(key_is_down(KEY_DOWN)) {
		bee.orientation = SOUTH;
		bee.current_animation = BEE_FLYING;
		bee.y_speed+= delta_speed;
	}
	else {
		bee.y_speed = closer_to_zero(bee.y_speed);
	}

	if(key_is_down(KEY_LEFT)) {
		bee.orientation = WEST;
		bee.current_animation = BEE_FLYING;
		bee.x_speed-= delta_speed;
	}
	else if(key_is_down(KEY_RIGHT)) {
		bee.orientation = EAST;
		bee.current_animation = BEE_FLYING;
		bee.x_speed+= delta_speed;
	}
	else {
		bee.x_speed = closer_to_zero(bee.x_speed);
	}

	bee.x_speed = cap_speed_and_apply_friction(bee.x_speed, accel_friction, bee.max_speed);
 	bee.y_speed = cap_speed_and_apply_friction(bee.y_speed, accel_friction, bee.max_speed);

	if (bee.x+bee.x_speed > 0-32 && bee.x+bee.x_speed < SCREEN_WIDTH-32)
		bee.x += bee.x_speed;
	if (bee.y+bee.y_speed > 0-32 && bee.y+bee.y_speed < SCREEN_HEIGHT-32)
		bee.y += bee.y_speed;
	
	/*
	bee.x += key_tri_horz();
	bee.y += key_tri_vert();
	*/

	// WIP (add loop marker to wav; stop loop if pause or else)
	// if (prev_mvt != BEE_FLYING && bee.current_animation == BEE_FLYING)
	// {
	// 	sfx_beeloop_handle = play_sfx(&sfx_beeloop);
	// }

	// if (prev_mvt == BEE_FLYING && bee.current_animation != BEE_FLYING)
	// {
	// 	mmEffectCancel(sfx_beeloop_handle);
	// }

	// prev_mvt = bee.current_animation;

}