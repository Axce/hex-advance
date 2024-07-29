#include <tonc.h>

#include "coordinates.h"

/*                                              x->
               <-y  ___  x->                     ___________________
    (y,x)       ___/0,0\___                   y |0,0|0,1|0,2|0,3| …
            ___/1,0\___/0,1\___               | |1,0|1,1|1,2| … |  
        ___/2,0\___/1,1\___/0,2\___           v |2,0|2,1| … |   |  
    ___/3,0\___/2,1\___/1,2\___/0,3\___         |3,0| … |   |   |  
___/ … \___/ … \___/ … \___/ … \___/ … \___     | … |   |   |   |  
*/
int board[11][11] =
{
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0},
};

bool is_stone_in_board(Board_XY board_xy) {
    int y = board_xy.y;
    int x = board_xy.x;

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

    board_xy.x = (y-13)/12 + (x-112)/20;
    board_xy.y = (y-13)/12 - (x-112)/20;

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