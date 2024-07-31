#include "coordinates.h"
#include "cpu_player.h"
#include "game.h"
#include "toolbox.h"

Board_XY cpu_find_next_move()
{
    return random_ai();
}

Board_XY random_ai()
{
    sqran(global_frame);

    int x,y;

    do
    {
        x = qran_range(0,11);
        y = qran_range(0,11);
    } while(board[y][x]);
    
    return new_board_xy(x,y);
}