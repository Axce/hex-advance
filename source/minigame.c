#include "PlayerSprite.h"
#include "graphics.h"
#include "game.h"
#include <string.h>
#include "coordinates.h"
#include "cpu_player.h"
#include "minigame.h"
#include "mem_management.h"
#include "menus.h"
#include "audio.h"

bool minigame_won = false;
bool minigame_lost = false;

Board_XY minigame_stone_put_pos;

Board_XY larva_board_xy;

void restart_minigame()
{
    minigame_won = false;
    minigame_lost = false;

    memset(board, 0, sizeof(board));
    init_stones_sprites();

    // placeholder
    sqran(global_frame);
    // int starting_stones = qran_range(10,15);
    int starting_stones = BOARD_SIZE + 4;
    for (int i = 0; i < starting_stones; i++)
    {
        int x = qran_range(0, BOARD_SIZE);
        int y = qran_range(0, BOARD_SIZE);
        board[y][x] = PLAYER_1_BLACK;
        update_stones_sprites(PLAYER_1_BLACK, new_board_xy(x, y));
    }

    board[BOARD_SIZE/2][BOARD_SIZE/2] = LARVA;
    larva_board_xy = new_board_xy(BOARD_SIZE/2, BOARD_SIZE/2);
    update_stones_sprites(PLAYER_1_BLACK, new_board_xy(BOARD_SIZE/2, BOARD_SIZE/2));  // in case need to remove stone at center

    current_player = PLAYER_1_BLACK;
    switch_player_graphics();
    winner = NOBODY;
    obj_hide(&obj_buffer[OAM_MENUS]);          //hide win text
    obj_unhide(bee.obj, DCNT_MODE0);
    update_bee_sprite();

    game_state = MINIGAME;
}

void minigame_player_play() {

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
    minigame_stone_put_pos = board_xy;
    game_state = MINIGAME_PUTTING_STONE;
    mmEffectEx(&sfx_bee);
}

void larva_play()
{

    Board_XY new_pos = larva_find_next_move();

    if (minigame_won)
    {
        init_menu(MENU_YOUWIN);
        return;
    }

    mmEffectEx(&sfx_larva);

    larva_move(new_pos);
}

// https://stackoverflow.com/a/52999191 was wrong but kinda helpful
Board_XY larva_find_next_move()
{
    int distance_to_win[MAX_BOARD_SIZE][MAX_BOARD_SIZE] = {0};
    int how_many_routes[MAX_BOARD_SIZE][MAX_BOARD_SIZE] = {0};

    int neighbor_Ys[6] = {-1,-1, 0, 0,+1,+1};
    int neighbor_Xs[6] = {-1, 0,-1,+1, 0,+1};

    Board_XY queue[MAX_BOARD_SIZE*MAX_BOARD_SIZE] = {0};
    int write_cursor = 0;
    int read_cursor = 0;

    // set the sides
    for(int iy = 0; iy < BOARD_SIZE; iy++)
    {
        if (board[iy][0] != PLAYER_1_BLACK)
        {
            distance_to_win[iy][0] = 1;
            how_many_routes[iy][0] = 2;
            queue[write_cursor++] = new_board_xy(0, iy);
        }

        if (board[iy][BOARD_SIZE-1] != PLAYER_1_BLACK)
        {
            distance_to_win[iy][BOARD_SIZE-1] = 1;
            how_many_routes[iy][BOARD_SIZE-1] = 2;
            queue[write_cursor++] = new_board_xy(BOARD_SIZE-1, iy);
        }
    }

    for(int ix = 1; ix < BOARD_SIZE-1; ix++)
    {
        if (board[0][ix] != PLAYER_1_BLACK)
        {
            distance_to_win[0][ix] = 1;
            how_many_routes[0][ix] = 2;
            queue[write_cursor++] = new_board_xy(ix, 0);
        }

        if (board[BOARD_SIZE-1][ix] != PLAYER_1_BLACK)
        {
            distance_to_win[BOARD_SIZE-1][ix] = 1;
            how_many_routes[BOARD_SIZE-1][ix] = 2;
            queue[write_cursor++] = new_board_xy(ix, BOARD_SIZE-1);
        }
    }

    // make distances / route BFS
    while (read_cursor < write_cursor)
    {
        Board_XY current_node = queue[read_cursor++];
        int x = current_node.x;
        int y = current_node.y;

        for (int ni = 0; ni < 6; ni++)                          // pour chaque voisin potentiel
        {
            int nx = x + neighbor_Xs[ni];
            int ny = y + neighbor_Ys[ni];
            if (is_in_board(nx, ny))
            {
                if (board[ny][nx] != PLAYER_1_BLACK              // si vide ou larve,
                    && distance_to_win[ny][nx] == 0 )               // et si pas encore visité
                {
                    queue[write_cursor++] = new_board_xy(nx, ny);   // on l'ajoute à la queue
                    distance_to_win[ny][nx] = distance_to_win[y][x] + 1;             // on marque sa distance,
                }
                                                            //// on en profite pour calculer le nombre de routes
                if (distance_to_win[ny][nx] > 0
                    && distance_to_win[ny][nx] < distance_to_win[y][x])    //// et si suite de la route
                {
                    how_many_routes[y][x] += how_many_routes[ny][nx];
                }
            }

        }


        // check endgame
        // if (board[y][x] == LARVA)
        // {
        //     if (distance_to_win[y][x] == 0)
        //         while(1);   // win
        //     if (distance_to_win[y][x] == 1)
        //         while(1);   // lose
        // }

    }

    int best_score = -1;
    int best_ni = 0;

    for (int ni = 0; ni < 6; ni++)      // pour chaque case adjacente à la larve
    {
        int x = larva_board_xy.x;
        int y = larva_board_xy.y;
        int nx = x + neighbor_Xs[ni];
        int ny = y + neighbor_Ys[ni];
        
        if ( /*is_in_board(nx, ny)*/ // has to be
            distance_to_win[ny][nx] == distance_to_win[y][x] - 1)
        {
            int routes = how_many_routes[ny][nx];
            if (routes > best_score)
            {
                best_score = routes;
                best_ni = ni;
            }
        }
    }

    if (best_score == -1)
    {
        minigame_won = true;
    }

    int nx = larva_board_xy.x + neighbor_Xs[best_ni];
    int ny = larva_board_xy.y + neighbor_Ys[best_ni];

    return new_board_xy(nx,ny);

}

void larva_move(Board_XY new_pos)
{
    board[larva_board_xy.y][larva_board_xy.x] = 0;
    larva_board_xy.x = new_pos.x;
    larva_board_xy.y = new_pos.y;
    board[larva_board_xy.y][larva_board_xy.x] = LARVA;
}

void minigame_end_turn() {

    mmEffectEx(&sfx_put_stone);

    update_stones_sprites(current_player, minigame_stone_put_pos);
    
    game_state = MINIGAME;

    larva_play();
}

void check_lose()
{
    if (larva_board_xy.x == 0 || larva_board_xy.x == BOARD_SIZE-1 || larva_board_xy.y == 0 || larva_board_xy.y == BOARD_SIZE-1)
    {
        minigame_lost = true;
    }
}