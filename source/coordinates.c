#include <tonc.h>

#include "coordinates.h"
#include "game.h"

bool is_stone_in_board(Screen_XY screen_xy) {

    Board_XY b_xy = to_board_xy(screen_xy);

    int y = b_xy.y;
    int x = b_xy.x;

    return is_in_board(x, y);

	// if (5*y - 3*x < -271)	//test si y_orth < 0
	// 	return false;
	// if (5*y - 3*x > 329)	//test si y_orth > 10
	// 	return false;
	// if (5*y + 3*x < 401)	//test si x_orth < 0
	// 	return false;
	// if (5*y + 3*x > 1001)	//test si x_orth > 10
	// 	return false;

	// return true;
}

Board_XY to_board_xy(Screen_XY screen_xy)
{
    int x = screen_xy.x;
    int y = screen_xy.y;
    
    Board_XY board_xy;

    int top = 79 - 6*BOARD_SIZE;

    // do not simplify (working with integers)
    board_xy.x = ((y-top)/6 + (x-112)/10) / 2;
    board_xy.y = ((y-top)/6 - (x-112)/10) / 2;

    return board_xy;
}

Screen_XY to_screen_xy(Board_XY board_xy)
{
    int x = board_xy.x;
    int y = board_xy.y;
    
    Screen_XY screen_xy;

    int top = 79 - 6*BOARD_SIZE;

    screen_xy.x = 10 * (x-y) + 112;
    screen_xy.y = 6 * (x+y) + top;

    return screen_xy;
}

Screen_XY get_sprite_position(OBJ_ATTR* sprite)
{
    Screen_XY pos;
    pos.y = sprite->attr0 & ATTR0_Y_MASK;
    pos.x = sprite->attr1 & ATTR1_X_MASK;
    return pos;
}

Board_XY new_board_xy(int x, int y)
{
    Board_XY xy;
    xy.x = x;
    xy.y = y;
    return xy;
}

Screen_XY new_screen_xy(int x, int y)
{
    Screen_XY xy;
    xy.x = x;
    xy.y = y;
    return xy;
}
