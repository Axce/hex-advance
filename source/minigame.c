#include "PlayerSprite.h"
#include "graphics.h"
#include "game.h"
#include <string.h>
#include "coordinates.h"
#include "cpu_player.h"
#include "minigame.h"

Board_XY minigame_stone_put_pos;

Board_XY larva_board_xy;

void restart_minigame()
{
    memset(board, 0, sizeof(board));


    board[5][5] = LARVA;
    larva_board_xy = new_board_xy(5,5);

    init_stones_sprites();
    // placeholder
    sqran(global_frame);
    for (int i = 0; i < 5; i++)
    {
        int x = qran_range(0, BOARD_SIZE);
        int y = qran_range(0, BOARD_SIZE);
        board[y][x] = PLAYER_1_BLACK;
        update_stones_sprites(PLAYER_1_BLACK, new_board_xy(x, y));
    }

    current_player = PLAYER_1_BLACK;
    switch_player_graphics();
    winner = 0;
    obj_hide(&obj_buffer[74]);          //hide win text
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
    
}

void larva_play()
{

    Board_XY new_pos = larva_find_next_move();

    larva_move(new_pos);
}

// https://stackoverflow.com/a/52999191
Board_XY larva_find_next_move()
{
    int distance_to_win[BOARD_SIZE][BOARD_SIZE] = {0};
    int how_many_routes[BOARD_SIZE][BOARD_SIZE] = {0};
    int distance, routes;

    int neighbor_Ys[6] = {-1,-1, 0, 0,+1,+1};
    int neighbor_Xs[6] = {-1, 0,-1,+1, 0,+1};

    Board_XY queue[BOARD_SIZE*BOARD_SIZE] = {0};
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
            if ( is_in_board(nx, ny)                            // si dans le board,
                && board[ny][nx] != PLAYER_1_BLACK              // si vide ou larve,
                && distance_to_win[ny][nx] == 0 )               // et si pas encore visité
            {
                queue[write_cursor++] = new_board_xy(nx, ny);   // on l'ajoute à la queue
                distance = distance_to_win[y][x] + 1;
                distance_to_win[ny][nx] = distance;             // on marque sa distance,
                
                routes = 0;
                for (int nni = 0; nni < 6; nni++)                   //// pour chaque voisin du nouveau nœud
                {
                    int nnx = nx + neighbor_Xs[nni];
                    int nny = ny + neighbor_Ys[nni];
                    if (is_in_board(nnx, nny)                       //// si dans le board,
                        && distance_to_win[nny][nnx] > 0
                        && distance_to_win[nny][nnx] < distance)    //// et si suite de la route
                    {
                        routes += how_many_routes[nny][nnx];
                    }
                }
                how_many_routes[ny][nx] = routes;               // et son nb de routes
            }
        }
    }

    int best_score = -1;
    int best_ni;
    int score;
    for (int ni = 0; ni < 6; ni++)      // pour chaque case adjacente à la larve
    {
        int nx = larva_board_xy.x + neighbor_Xs[ni];
        int ny = larva_board_xy.y + neighbor_Ys[ni];

        distance = distance_to_win[ny][nx];
        routes = how_many_routes[ny][nx];
        score = routes * 360360 / distance; // 360360 is divisible by [1…15], for higher chance working with integers only
        if (score > best_score)
        {
            best_score = score;
            best_ni = ni;
        }
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
    
    update_stones_sprites(current_player, minigame_stone_put_pos);

    if (has_won(current_player) != NULL)
    {
        winner = current_player;
        game_state = MINIGAME_ENDED;
        display_victory();
    }
    else
    {
		larva_play();
        game_state = MINIGAME;
    }
}