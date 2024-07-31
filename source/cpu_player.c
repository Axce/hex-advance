#include "coordinates.h"
#include "cpu_player.h"
#include "game.h"
#include "toolbox.h"

// According to :
// https://www.hexwiki.net/index.php/Swap#Size_11
// https://www.hexwiki.net/index.php/Openings_on_11_x_11
// 
// Black plays first
//  0 = fair
//  1 = favorable to black, white should swap
//  2 = bad move from black, white should play
// 
// Can be read as "which player is likely to win"
const int swap_map[11][11] =
{
    {0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 2},
    {2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2},
    {2, 0, 1, 1, 1, 1, 1, 1, 1, 0, 2},
    {2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2},
    {2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2},
    {2, 2, 0, 1, 1, 1, 1, 1, 0, 2, 2},
    {2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2},
    {2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2},
    {2, 0, 1, 1, 1, 1, 1, 1, 1, 0, 2},
    {2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2},
    {2, 2, 0, 0, 1, 0, 0, 0, 0, 1, 0},
};

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