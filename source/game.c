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

    if (!stone_put)
    {
        return;
    }
    
    if (has_won(current_player))
    {
        while(1);
    }

    switch_player();
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

    /*
    if (current_player == PLAYER_1_BLACK)
    {
        current_player = PLAYER_2_WHITE;
    }
    else if (current_player == PLAYER_2_WHITE)
    {
        current_player = PLAYER_1_BLACK;
    }
    */

    current_player ^= 0b11; // switch between 1 and 2 (0b01 and 0b10) (doesn't work)

    switch_player_graphics();
}

bool has_won(Player player) {

    // 0 = not visited
    // 1 = to visit
    // 2 = visited
    int visit_board[11][11] =
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

    Board_XY parents[11][11] = { 0 };

    int neighbor_Ys[6] = {-1,-1, 0, 0,+1,+1};
    int neighbor_Xs[6] = {-1, 0,-1,+1, 0,+1};

    Board_XY queue[121] = {0};
    int write_cursor = 0;
    int read_cursor = 0;

    // path from x=0 to x=10
    if (player == PLAYER_1_BLACK)
    {

        for (int iy = 0 ; iy < 11 ; iy++)
        {
            if (board[iy][0] == player)
            {
                visit_board[iy][0] = 1;
                queue[write_cursor++] = new_board_xy(0, iy);
            }
        }
    }
    // path from y=0 to y=10
    if (player == PLAYER_2_WHITE)
    {
        for (int ix = 0 ; ix < 11 ; ix++)
        {
            if (board[0][ix] == player)
            {
                visit_board[0][ix] = 1;
                queue[write_cursor++] = new_board_xy(ix, 0);
            }
        }
    }

    while (read_cursor < write_cursor)
    {
        Board_XY current_node = queue[read_cursor++];
        int x = current_node.x;
        int y = current_node.y;
        
        for (int ni = 0; ni < 6; ni++)                  // pour chaque voisin potentiel
        {
            int nx = x + neighbor_Xs[ni];
            int ny = y + neighbor_Ys[ni];
            
            if ( nx>=0 && nx<11 && ny>=0 && ny<11 &&    // si dans le board,
                board[ny][nx] == player &&              // si un pion du player est là,
                visit_board[ny][nx] == 0)               // et si pas encore visité
            {
                parents[ny][nx] = new_board_xy(x, y);           // on note son node parent
                
                if (player == PLAYER_1_BLACK && nx==10)
                {
                    return true;
                }
                if (player == PLAYER_2_WHITE && ny==10)
                {
                    return true;
                }

                queue[write_cursor++] = new_board_xy(nx, ny);   // on l'ajoute à la queue
                visit_board[ny][nx] = 1;                        // on le marque "à visiter"
                
            }
            
        }

        visit_board[y][x] = 2;
        
    }
    
    

    return false;

}