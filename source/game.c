#include "PlayerSprite.h"
#include "graphics.h"
#include "game.h"
#include <string.h>
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

void play() {

    Screen_XY ghost_stone_screen_xy = get_sprite_position(ghost_stone);
    
    if (!is_stone_in_board(ghost_stone_screen_xy))
    {
        return;
    }

    Board_XY board_xy = to_board_xy(ghost_stone_screen_xy);

    bool stone_put = put_stone(current_player, board_xy);

    if (stone_put)
    {
        switch_player();
    }
}

// board_xy parameter wants coordinates inside the board
// returns false if a stone is already here
// returns true if stone has been put successfully
bool put_stone(Player player, Board_XY board_XY)
{
    int x = board_XY.x;
    int y = board_XY.y;

    if (board[y][x] != 0)
    {
        return false;
    }

    board[y][x] = player; //(player == PLAYER_1_BLACK) ? 1 : 0;
    
    update_stones_sprites(player, board_XY);

    return true;
}

void switch_player() {

	current_player ^= 11; // switch between 1 and 2 (0b01 and 0b10)

    switch_player_graphics();
}

