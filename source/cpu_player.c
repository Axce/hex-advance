#include "coordinates.h"
#include "cpu_player.h"
#include "game.h"
#include "toolbox.h"
#include "audio.h"
#include "graphics.h"

const int direct_neighbors_y[6] =   {-1,-1, 0, 0, 1, 1};
const int direct_neighbors_x[6] =   {-1, 0,-1, 1, 0, 1};
const int direct_obstacle_1_y[6] =  { 0,-1,-1,-1, 0, 1};
const int direct_obstacle_1_x[6] =  {-1,-1,-1, 0,-1, 0};
const int direct_obstacle_2_y[6] =  {-1, 0, 1, 1, 1, 0};
const int direct_obstacle_2_x[6] =  { 0, 1, 0, 1, 1, 1};

const int bridge_neighbors_y[6] =   {-2,-1,-1,+1,+1,+2};
const int bridge_neighbors_x[6] =   {-1,-2,+1,-1,+2,+1};
const int bridge_obstacle_1_y[6] =  {-1,-1,-1,+1,+1,+1};
const int bridge_obstacle_1_x[6] =  {-1,-1, 0, 0,+1,+1};
const int bridge_obstacle_2_y[6] =  {-1, 0, 0, 0, 0,+1};
const int bridge_obstacle_2_x[6] =  { 0,-1,+1,-1,+1, 0};

const int enemy[3] = {NOBODY, PLAYER_2_WHITE, PLAYER_1_BLACK};

#define INFINITY 1000

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

int thinking_progress;
int thinking_progress_max;

int new_frame = false;

// clever hack to keep stuff RUNNING while thinking
void check_for_vblank()
{
    if (new_frame == false && REG_VCOUNT < 160)
    {
        new_frame = true;
    }

    if (new_frame == true && REG_VCOUNT >= 160)
    {
        mmFrame();
        global_frame++;
        key_poll();
        update_bee_thinking_position();
        update_bee_sprite();
		obj_copy(obj_mem, obj_buffer, OBJ_COUNT);
        new_frame = false;
    }

}

// makes a nice circle while thinking
void update_bee_thinking_position()
{
    int theta = thinking_progress * 0xFFFF / thinking_progress_max;

    // position
    int y = lu_sin(theta - 0x4000) >> 7;
    int x = lu_cos(theta - 0x4000) >> 7;
    bee.x = x + SCREEN_WIDTH/2-32;
    bee.y = y + SCREEN_HEIGHT/2-32;

    // orientation
    bee.orientation = EAST;
    if (theta > 0x1000)
        bee.orientation = SOUTH;
    if (theta > 0x5000)
        bee.orientation = WEST;
    if (theta > 0x9000)
        bee.orientation = NORTH;
    if (theta > 0xD000)
        bee.orientation = EAST;

}

Board_XY cpu_find_next_move()
{

    new_frame = false;  // DO NOT FORGET THIS before a loop with check_for_vblank();

    // simulate long thinking time
    // int i=5000;
    // while(i--)
    // {
    //     printf("lol");
    //     check_for_vblank();
    // }
    // return random_ai();

    return best_score_ai(board, PLAYER_2_WHITE);

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

Board_XY best_score_ai(int board[BOARD_SIZE][BOARD_SIZE], int player)
{
    thinking_progress = 0; // 1?
    thinking_progress_max = BOARD_SIZE * BOARD_SIZE;

    int best_score = -INFINITY;
    Board_XY best_moves[BOARD_SIZE*BOARD_SIZE]; // TODO get only if valid
    int write_cursor = 0;

    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (board[y][x] == 0)
            {
                board[y][x] = player;
                int score = least_moves_to_win(board, enemy[player], enemy[player]) - least_moves_to_win(board, player, enemy[player]);
                if (score == best_score)
                {
                    best_moves[write_cursor++] = new_board_xy(x, y);
                }
                if (score > best_score)
                {
                    best_score = score;
                    write_cursor = 0;
                    best_moves[write_cursor++] = new_board_xy(x, y);
                }
                board[y][x] = 0;
            }
            thinking_progress ++;
        }
    }

    return best_moves[qran_range(0, write_cursor)];
}


// si cette heuristique durait moins d'une frame, ça serait super (pour check_for_vblank)
// TODO check ziggurats
// knowing the next player to move can be useful to put some cases into perspective
int least_moves_to_win(int board[BOARD_SIZE][BOARD_SIZE], Player player, Player next_player) {

    // This returns the shortest amount of stones needed to connect the sides.
    // Uses 0-1 BFS algorithm.

    check_for_vblank();

    // not visited = 0
    // visited = 1
    int visited_board[BOARD_SIZE][BOARD_SIZE] = {0};
    uint path_length_board[BOARD_SIZE][BOARD_SIZE];

    memset32(path_length_board, 0xFFFFFFFF, BOARD_SIZE*BOARD_SIZE);

    // for (int i = 0; i < BOARD_SIZE; i++) {
    //     for (int j = 0; j < BOARD_SIZE; j++) {
    //         path_length_board[i][j] = INFINITY;     // SO SLOW : replace with memcpy 0xFFFF chaipaquoi
    //     }
    // }

    Board_XY nodes_queue_0[BOARD_SIZE * BOARD_SIZE] = {0};
    int write_cursor_0 = 0;
    int read_cursor_0 = 0;
    Board_XY nodes_queue_1[BOARD_SIZE * BOARD_SIZE] = {0};
    int write_cursor_1 = 0;
    int read_cursor_1 = 0;


    // FIRST: filling the queue with the cells next to sides

    // black : from x=0 to x=BOARD_SIZE-1
    if (player == PLAYER_1_BLACK) {
        
        // bridges to border
        for (int iy = 1; iy < BOARD_SIZE; iy++) {
            if (board[iy][1] == player) { // cost 0
                if (is_free_bridge(board, 1, iy, BN_TOPLEFT)) {
                    path_length_board[iy][1] = 0;
                    nodes_queue_0[write_cursor_0++] = new_board_xy(1, iy);
                    visited_board[iy][1] = 1;
                }
            }
            if (board[iy][1] == 0) { // cost 1
                if (is_free_bridge(board, 1, iy, BN_TOPLEFT)) {
                    path_length_board[iy][1] = 1;
                    nodes_queue_1[write_cursor_1++] = new_board_xy(1, iy);
                    visited_board[iy][1] = 1;
                }
            }
        }

        // direct neighbors to border
        for (int iy = 0; iy < BOARD_SIZE; iy++) {
            if (board[iy][0] == player) {   // cost 0
                path_length_board[iy][0] = 0;
                nodes_queue_0[write_cursor_0++] = new_board_xy(0, iy);
                visited_board[iy][0] = 1;
            }
            if (board[iy][0] == 0) {    // cost 1
                path_length_board[iy][0] = 1;
                nodes_queue_1[write_cursor_1++] = new_board_xy(0, iy);
                visited_board[iy][0] = 1;
            }
        }
    } else {    // white : from y=0 to y=BOARD_SIZE-1

        // bridges to border
        for (int ix = 1; ix < BOARD_SIZE; ix++) {
            if (board[1][ix] == player) {   // cost 0
                if (is_free_bridge(board, ix, 1, 1)) {
                    path_length_board[1][ix] = 0;
                    nodes_queue_0[write_cursor_0++] = new_board_xy(ix,  1);
                    visited_board[1][ix] = 1;
                }
            }
            if (board[1][ix] == 0) {    // cost 1
                if (is_free_bridge(board, ix, 1, 1)) {
                    path_length_board[1][ix] = 1;
                    nodes_queue_1[write_cursor_1++] = new_board_xy(ix,  1);
                    visited_board[1][ix] = 1;
                }
            }
        }

        // direct neighbors to border
        for (int ix = 0; ix < BOARD_SIZE; ix++) {
            if (board[0][ix] == player) {   // cost 0
                path_length_board[0][ix] = 0;
                nodes_queue_0[write_cursor_0++] = new_board_xy(ix,  0);
                visited_board[0][ix] = 1;
            }
            if (board[0][ix] == 0) {    // cost 1
                path_length_board[0][ix] = 1;
                nodes_queue_1[write_cursor_1++] = new_board_xy(ix,  0);
                visited_board[0][ix] = 1;
            }
        }
    }

    // THEN: proceed to 0-1 BFS

    while (1) {
        
        check_for_vblank();

        Board_XY current_node_xy;
        if (read_cursor_0 < write_cursor_0) {
            current_node_xy = nodes_queue_0[read_cursor_0++];
        } else if (read_cursor_1 < write_cursor_1) {
            current_node_xy = nodes_queue_1[read_cursor_1++];
        } else {
            return INFINITY; // no path
        }

        int x = current_node_xy.x;
        int y = current_node_xy.y;

        // found shortest path!
        if (is_connected_to_end_border(board, player, x, y)) {
            return path_length_board[y][x];
        }

        // for every bridge neighbor
        for (int ni = 0; ni < 6; ni++) {
            int nx = x + bridge_neighbors_x[ni];
            int ny = y + bridge_neighbors_y[ni];

            if (is_in_board(nx, ny) && visited_board[ny][nx] == 0 && is_free_bridge(board, x, y, ni)) {
                if (board[ny][nx] == player) {
                    path_length_board[ny][nx] = path_length_board[y][x] + 0;
                    nodes_queue_0[write_cursor_0++] = new_board_xy(nx, ny);
                    visited_board[ny][nx] = 1;
                } else if (board[ny][nx] == 0) {
                    path_length_board[ny][nx] = path_length_board[y][x] + 1;
                    nodes_queue_1[write_cursor_1++] = new_board_xy(nx, ny);
                    visited_board[ny][nx] = 1;
                }
            }
        }
        
        // for every direct neighbor
        for (int ni = 0; ni < 6; ni++) {
            int nx = x + direct_neighbors_x[ni];
            int ny = y + direct_neighbors_y[ni];
            if (is_in_board(nx, ny) && visited_board[ny][nx] == 0 && !is_blocked_by_enemy_bridge(board, enemy[player], next_player, x, y, ni)) {
                if (board[ny][nx] == player) {
                    path_length_board[ny][nx] = path_length_board[y][x] + 0;
                    nodes_queue_0[write_cursor_0++] = new_board_xy(nx, ny);
                    visited_board[ny][nx] = 1;
                } else if (board[ny][nx] == 0) {
                    path_length_board[ny][nx] = path_length_board[y][x] + 1;
                    nodes_queue_1[write_cursor_1++] = new_board_xy(nx, ny);
                    visited_board[ny][nx] = 1;
                }
            }
        }
    }
}

// int is_free_bridge(int board[BOARD_SIZE][BOARD_SIZE], int x, int y, int direction);
// int is_blocked_by_enemy_bridge(int board[BOARD_SIZE][BOARD_SIZE], int enemy, int x, int y, int direction);
// int is_owned_by(int board[BOARD_SIZE][BOARD_SIZE], int x, int y);
// int is_connected_to_end_border(int board[BOARD_SIZE][BOARD_SIZE], int player, int x, int y);


// assuming 2 cells inbounds
bool is_free_bridge(int board[BOARD_SIZE][BOARD_SIZE], int x, int y, enum BRIDGE_NEIGHBORS ni) {
    int x1 = x + bridge_obstacle_1_x[ni];
    int y1 = y + bridge_obstacle_1_y[ni];
    int x2 = x + bridge_obstacle_2_x[ni];
    int y2 = y + bridge_obstacle_2_y[ni];
    return (board[y1][x1] == 0) && (board[y2][x2] == 0);
}

int is_owned_by(int board[BOARD_SIZE][BOARD_SIZE], int x, int y) {
    if (x < 0 || x >= BOARD_SIZE) { // out of bounds for black
        if (y >= 0 && y < BOARD_SIZE) {
            return PLAYER_1_BLACK;
        } else {
            return NOBODY;
        }
    }
    if (y < 0 || y >= BOARD_SIZE) { // out of bounds for white
        if (x >= 0 && x < BOARD_SIZE) {
            return PLAYER_2_WHITE;
        } else {
            return NOBODY;
        }
    }
    return board[y][x];
}

bool is_blocked_by_enemy_bridge(int board[BOARD_SIZE][BOARD_SIZE], Player enemy, Player next_player, int x, int y, enum DIRECT_NEIGHBORS ni) {
    int x1 = x + direct_obstacle_1_x[ni];
    int y1 = y + direct_obstacle_1_y[ni];
    int x2 = x + direct_obstacle_2_x[ni];
    int y2 = y + direct_obstacle_2_y[ni];
    int nx = x + direct_neighbors_x[ni];
    int ny = y + direct_neighbors_y[ni];

    // potential blocking bridge scenario
    if (is_owned_by(board, x1, y1) == enemy
        && is_owned_by(board, x2, y2) == enemy)
    {
        // if no friend stones present, the bridge is blocking.
        if (board[y][x] == 0
            && board[ny][nx] == 0)
        {
            return true;
        }

        // if we know it's enemy's turn after checking this position,
        // even if one friend stone is present, enemy can block.
        if (next_player == enemy)
        {
            if (board[y][x] == 0
                || board[ny][nx] == 0)
            {
                return true;
            }
        }
    }
    
    return false;

}

bool is_connected_to_end_border(int board[BOARD_SIZE][BOARD_SIZE], int player, int x, int y) {
    if (player == PLAYER_1_BLACK) {
        if (x == BOARD_SIZE - 1) { // direct
            return true;
        }
        if (x == BOARD_SIZE - 2) { // bridge
            if (y < BOARD_SIZE - 1) {
                if (is_free_bridge(board, x, y, BN_BOTRIGHT)) { // 5th neighbor in LUT
                    return true;
                }
            }
        }
    }

    if (player == PLAYER_2_WHITE) {
        if (y == BOARD_SIZE - 1) {  // direct
            return true;
        }
        if (y == BOARD_SIZE - 2) { // bridge
            if (x < BOARD_SIZE - 1) {
                if (is_free_bridge(board, x, y, BN_BOTLEFT)) { // 6th neighbor in LUT
                    return true;
                }
            }
        }
    }
    return false;
}
