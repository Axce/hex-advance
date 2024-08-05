#include "PlayerSprite.h"
#include "graphics.h"
#include "game.h"
#include <string.h>
#include "coordinates.h"
#include "cpu_player.h"
#include "mem_management.h"

/*                                              x->
               <-y  ___  x->                     ___________________
    (y,x)       ___/0,0\___                   y |0,0|0,1|0,2|0,3| …
            ___/1,0\___/0,1\___               | |1,0|1,1|1,2| … |  
        ___/2,0\___/1,1\___/0,2\___           v |2,0|2,1| … |   |  
    ___/3,0\___/2,1\___/1,2\___/0,3\___         |3,0| … |   |   |  
___/ … \___/ … \___/ … \___/ … \___/ … \___     | … |   |   |   |  
*/
int board[BOARD_SIZE][BOARD_SIZE] =
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
    {0,0,0,0,0,0,0,0,0,0,0}
};

Player winner = 0;

Board_XY stone_put_pos;

void player_play() {

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

    putting_stone_delay = bee.anim_frames * bee.anim_delay;
    stone_put_pos = board_xy;
    game_state = PUTTING_STONE;
    
}

void cpu_play() {

    Board_XY board_xy = cpu_find_next_move();

    bool stone_put = put_stone(current_player, board_xy);

    if (!stone_put)
    {
        while(1); // not supposed to happen
    }

    putting_stone_delay = bee.anim_frames * bee.anim_delay;
    stone_put_pos = board_xy;

    Screen_XY stone_screen_pos = to_screen_xy(stone_put_pos);

    bee.x = stone_screen_pos.x - 32;
    bee.y = stone_screen_pos.y - 32;

    game_state = PUTTING_STONE;
    
}

void end_turn() {
    
    update_stones_sprites(current_player, stone_put_pos);

    if (has_won(current_player) != NULL)
    {
        winner = current_player;
        game_state = GAME_ENDED;
        display_victory();
    }
    else
    {
        game_state = IN_GAME;
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

Board_XY* reconstruct_path(Board_XY end, Board_XY parents[11][11])
{
    // PLACEHOLDER
    return (Board_XY*)1;
}

Board_XY* has_won(Player player) {

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

    Board_XY parents[BOARD_SIZE][BOARD_SIZE] = { 0 };

    int neighbor_Ys[6] = {-1,-1, 0, 0,+1,+1};
    int neighbor_Xs[6] = {-1, 0,-1,+1, 0,+1};

    Board_XY queue[BOARD_SIZE*BOARD_SIZE] = {0};
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
            
            if ( is_in_board(nx, ny) &&    // si dans le board,
                board[ny][nx] == player &&              // si un pion du player est là,
                visit_board[ny][nx] == 0)               // et si pas encore visité
            {
                parents[ny][nx] = new_board_xy(x, y);           // on note son node parent
                
                if (player == PLAYER_1_BLACK && nx==10)
                {
                    return reconstruct_path(new_board_xy(nx,ny), parents);
                }
                if (player == PLAYER_2_WHITE && ny==10)
                {
                    return reconstruct_path(new_board_xy(nx,ny), parents);
                }

                queue[write_cursor++] = new_board_xy(nx, ny);   // on l'ajoute à la queue
                visit_board[ny][nx] = 1;                        // on le marque "à visiter"
                
            }
            
        }

        visit_board[y][x] = 2;
        
    }
    
    

    return NULL;

}

void restart_game()
{
    memset(board, 0, sizeof(board));
    init_stones_sprites();
    current_player = PLAYER_1_BLACK;
    switch_player_graphics();
    winner = 0;
    obj_hide(&obj_buffer[OAM_MENUS]);          //hide win text
    obj_unhide(bee.obj, DCNT_MODE0);
    update_bee_sprite();

    game_state = IN_GAME;
}