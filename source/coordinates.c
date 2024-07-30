#include <tonc.h>

#include "coordinates.h"

bool is_stone_in_board(Screen_XY screen_xy) {
    int y = screen_xy.y;
    int x = screen_xy.x;

	if (5*y - 3*x < -271)	//test si y_orth < 0
		return false;
	if (5*y - 3*x > 329)	//test si y_orth > 10
		return false;
	if (5*y + 3*x < 401)	//test si x_orth < 0
		return false;
	if (5*y + 3*x > 1001)	//test si x_orth > 10
		return false;

	return true;
}

Board_XY to_board_xy(Screen_XY screen_xy)
{
    int x = screen_xy.x;
    int y = screen_xy.y;
    
    Board_XY board_xy;

    // do not simplify (working with integers)
    board_xy.x = ((y-13)/6 + (x-112)/10) / 2;
    board_xy.y = ((y-13)/6 - (x-112)/10) / 2;

    return board_xy;
}

Screen_XY to_screen_xy(Board_XY board_xy)
{
    int x = board_xy.x;
    int y = board_xy.y;
    
    Screen_XY screen_xy;

    screen_xy.x = 10 * (x-y) + 112;
    screen_xy.y = 6 * (x+y) + 13;

    return screen_xy;
}

Screen_XY get_sprite_position(OBJ_ATTR* sprite)
{
    Screen_XY pos;
    pos.y = sprite->attr0 & ATTR0_Y_MASK;
    pos.x = sprite->attr1 & ATTR1_X_MASK;
    return pos;
}