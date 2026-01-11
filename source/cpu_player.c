#include "coordinates.h"
#include "cpu_player.h"
#include "game.h"
#include "toolbox.h"
#include "audio.h"
#include "graphics.h"

#define INFINITY INT16_MAX

#define MAXING_PLAYER PLAYER_1_BLACK
#define MINING_PLAYER PLAYER_2_WHITE


// #define MCTS_MAX_NODES       64      // à ajuster selon la RAM
// #define MCTS_MAX_CHILDREN    32        // nombre max de coups possibles considérés
// #define MCTS_SIMULATIONS     15      // ~nombre total de simulations en 10 s (à calibrer)
// #define MCTS_EXPLORE_CONST   400       // C pour UCB1, en "fixed-point" entier



#define MCTS_MAX_NODES       128
#define MCTS_MAX_CHILDREN    8
#define MCTS_SIMULATIONS     10
#define MCTS_EXPLORE_CONST   200


typedef struct {
    // move that led to this node (from parent)
    int move_x;
    int move_y;
    Player player_to_move;   // qui doit jouer dans CET état

    int parent;              // index dans le pool, -1 pour la racine
    int first_child;         // index du 1er enfant dans le pool, -1 si feuille
    int child_count;

    int visits;              // nombre de simulations passées par ce noeud
    int total_value;         // somme des résultats (par ex : 0..1000) du point de vue d’un joueur de référence
} MCTS_Node;

static MCTS_Node mcts_nodes[MCTS_MAX_NODES];
static int mcts_node_count;


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

// black top left
const Board_XY black_top_ziggurat_center[6] =
{
    {-2,0}, {-2,-1}, {-2,-2},
        {-1,0}, {-1,-1},
            {0,0}
};
const Board_XY black_top_ziggurat_rightside[3] =
{
                        {-2,-3},
                    {-1,-2},
                {0,-1}
};
const Board_XY black_top_ziggurat_leftside[3] =
{
{-2,+1},
    {-1,+1},
        {0,+1}
};

// white top right
const Board_XY white_top_ziggurat_center[6] =
{
    {-2,-2}, {-1,-2}, {0,-2},
        {-1,-1}, {0,-1},
            {0,0}
};
const Board_XY white_top_ziggurat_rightside[3] =
{
                        {+1,-2},
                    {+1,-1},
                {+1,0}
};
const Board_XY white_top_ziggurat_leftside[3] =
{
{-3,-2},
    {-2,-1},
        {-1,0}
};

// white bot left
const Board_XY white_bot_ziggurat_center[6] =
{
            {0,0},
        {0, +1}, {+1,+1},
    {0, +2}, {+1,+2}, {+2,+2}
};
const Board_XY white_bot_ziggurat_rightside[3] =
{
                {+1,0},
                    {+2,+1},
                        {+3,+2}
};
const Board_XY white_bot_ziggurat_leftside[3] =
{
        {-1,0},
    {-1,+1},
{-1,+2}
};

// black bot right
const Board_XY black_bot_ziggurat_center[6] =
{
            {0,0},
        {+1,+1}, {+1,0},
    {+2,+2}, {+2,+1}, {+2,0}
};
const Board_XY black_bot_ziggurat_rightside[3] =
{
                {0,-1},
                    {+1,-1},
                        {+2,-1}
};
const Board_XY black_bot_ziggurat_leftside[3] =
{
        {0,+1},
    {+1,+2},
{+2,+3}
};

const int get_enemy[3] = {NOBODY, PLAYER_2_WHITE, PLAYER_1_BLACK};


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
// const int swap_map_11[11][11] =
// {
//     {0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 2},
//     {2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2},
//     {2, 0, 1, 1, 1, 1, 1, 1, 1, 0, 2},
//     {2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2},
//     {2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2},
//     {2, 2, 0, 1, 1, 1, 1, 1, 0, 2, 2},
//     {2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2},
//     {2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2},
//     {2, 0, 1, 1, 1, 1, 1, 1, 1, 0, 2},
//     {2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2},
//     {2, 2, 0, 0, 1, 0, 0, 0, 0, 1, 0},
// };

int thinking_progress = 0;
int thinking_progress_max = MCTS_SIMULATIONS * 20;

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
    thinking_progress = 0; // used to tell that the bee isn’t thinking anymore

    // Blanc = IA, adapté si besoin
    Board_XY next_move = mcts_find_next_move(board, PLAYER_2_WHITE);

    thinking_progress = 0; // used to tell that the bee isn’t thinking anymore

    return next_move;
}

// lmtw = least moves to win
// Considers best score = enemy's lmtw - own's lmtw
Board_XY best_score_ai(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int player)
{
    thinking_progress = 0; // 1?
    thinking_progress_max = BOARD_SIZE * BOARD_SIZE;

    int best_score = -INFINITY;
    Board_XY best_moves[MAX_BOARD_SIZE*MAX_BOARD_SIZE]; // TODO get only if valid
    int write_cursor = 0;

    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (board[y][x] == 0)
            {
                board[y][x] = player;

                int lmtw_black = least_moves_to_win(board, get_enemy[player], get_enemy[player]);
                int lmtw_white = least_moves_to_win(board, player, get_enemy[player]);

                int score = lmtw_black - lmtw_white;
                
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

// lmtw = least moves to win
// Only considers own's best (smallest) lmtw, and only then considers enemy's greatest lmtw
// Its playing "feels" more logical
Board_XY best_own_score_ai(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int player)
{
    thinking_progress = 0; // 1?
    thinking_progress_max = BOARD_SIZE * BOARD_SIZE;

    int best_own_lmtw = INFINITY;   // we want to min it
    int worst_enemy_lmtw = -INFINITY;   // we want to max it

    Board_XY best_moves[MAX_BOARD_SIZE*MAX_BOARD_SIZE]; // TODO get only if valid
    int write_cursor = 0;

    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (board[y][x] == 0)
            {
                // simulate put the stone
                board[y][x] = player;

                int lmtw_own = least_moves_to_win(board, player, get_enemy[player]);

                if (lmtw_own < best_own_lmtw)
                {
                    best_own_lmtw = lmtw_own;
                    worst_enemy_lmtw = least_moves_to_win(board, get_enemy[player], get_enemy[player]);
                    write_cursor = 0;
                    best_moves[write_cursor++] = new_board_xy(x, y);
                }
                else if (lmtw_own == best_own_lmtw)
                {
                    int lmtw_enemy = least_moves_to_win(board, get_enemy[player], get_enemy[player]);

                    if (lmtw_enemy > worst_enemy_lmtw)
                    {
                        worst_enemy_lmtw = lmtw_enemy;
                        write_cursor = 0;
                        best_moves[write_cursor++] = new_board_xy(x, y);
                    }
                    else if (lmtw_enemy == worst_enemy_lmtw)
                    {
                        best_moves[write_cursor++] = new_board_xy(x, y);
                    }

                }

                // remove the simulated stone
                board[y][x] = 0;
            }
            thinking_progress ++;
        }
    }

    thinking_progress = 0;  // used by onVBlank to know bee is thinking

    return best_moves[qran_range(0, write_cursor)];
}

//////////////////////////////////////////////////////
// TODO: triangles (double bridges) are not ZERO !! //
//////////////////////////////////////////////////////

// knowing the next player to move can be useful to put some cases into perspective
int least_moves_to_win(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], Player player, Player next_player) {

    // This returns the shortest amount of stones needed to connect the sides.
    // Uses 0-1 BFS algorithm.

    // not visited = 0
    // visited = 1
    int visited_board[MAX_BOARD_SIZE][MAX_BOARD_SIZE] = {0};
    uint path_length_board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];

    memset32(path_length_board, 0xFFFFFFFF, MAX_BOARD_SIZE*MAX_BOARD_SIZE);

    // for (int i = 0; i < BOARD_SIZE; i++) {
    //     for (int j = 0; j < BOARD_SIZE; j++) {
    //         path_length_board[i][j] = INFINITY;     // SO SLOW : replace with memcpy 0xFFFF chaipaquoi
    //     }
    // }

    Board_XY nodes_queue_0[MAX_BOARD_SIZE * MAX_BOARD_SIZE] = {0};
    int write_cursor_0 = 0;
    int read_cursor_0 = 0;
    Board_XY nodes_queue_1[MAX_BOARD_SIZE * MAX_BOARD_SIZE] = {0};
    int write_cursor_1 = 0;
    int read_cursor_1 = 0;


    // FIRST: filling the queue with the cells next to sides

    // black : wants to go from x=0 to x=BOARD_SIZE-1
    if (player == PLAYER_1_BLACK) {
        
        // zigs to border
        for (int iy = 2; iy < BOARD_SIZE; iy++) {
            if (board[iy][2] == player) { // cost 0
                if (is_free_ziggurat(board, 2, iy, player, next_player)) {
                    path_length_board[iy][2] = 0;
                    nodes_queue_0[write_cursor_0++] = new_board_xy(2, iy);
                    visited_board[iy][2] = 1;
                }
            }
            if (board[iy][2] == 0) { // cost 1
                if (is_free_ziggurat(board, 2, iy, player, next_player)) {
                    path_length_board[iy][2] = 1;
                    nodes_queue_1[write_cursor_1++] = new_board_xy(2, iy);
                    visited_board[iy][2] = 1;
                }
            }
        }

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
    } else {    // white : wants to go from y=0 to y=BOARD_SIZE-1

        // zigs to border
        for (int ix = 2; ix < BOARD_SIZE; ix++) {
            if (board[2][ix] == player) {   // cost 0
                if (is_free_ziggurat(board, ix, 2, player, next_player)) {
                    path_length_board[2][ix] = 0;
                    nodes_queue_0[write_cursor_0++] = new_board_xy(ix,  2);
                    visited_board[2][ix] = 1;
                }
            }
            if (board[2][ix] == 0) {    // cost 1
                if (is_free_ziggurat(board, ix, 2, player, next_player)) {
                    path_length_board[2][ix] = 1;
                    nodes_queue_1[write_cursor_1++] = new_board_xy(ix,  2);
                    visited_board[2][ix] = 1;
                }
            }
        }

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
        if (is_connected_to_end_border(board, player, x, y, next_player)) {
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
            if (is_in_board(nx, ny) && visited_board[ny][nx] == 0 && !is_blocked_by_enemy_bridge(board, get_enemy[player], next_player, x, y, ni)) {
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

// works even with borders
bool is_free_bridge(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int x, int y, enum BRIDGE_NEIGHBORS ni) {
    int x1 = x + bridge_obstacle_1_x[ni];
    int y1 = y + bridge_obstacle_1_y[ni];
    int x2 = x + bridge_obstacle_2_x[ni];
    int y2 = y + bridge_obstacle_2_y[ni];
    return (is_owned_by(board, x1, y1) == NOBODY) && (is_owned_by(board, x2, y2) == NOBODY);
}

// checks if a stone is connected to its border via one of the two ziggurats possible
// considering next player to move. Breaking zero-sum principle ? Would not work with minimax ?
bool is_free_ziggurat(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int x, int y, Player player, Player next_player) {

    const Board_XY (*zig_center)[6];
    const Board_XY (*zig_leftside)[3];
    const Board_XY (*zig_rightside)[3];

    // if it's my turn, one enemy can be in ziggurat and it's still free
    // AKA if white AI plays in black human ziggurat, let’s assume black human will answer in ziggurat.
    int can_meet_one_enemy;
    if (next_player == player)
        can_meet_one_enemy = true;
    else
        can_meet_one_enemy = false;


    // knowing what zig we're talking about
    if (player == PLAYER_1_BLACK)   // black
    {
        if (x == 2) // top-left side
        {
            if (y < 2 || y >= BOARD_SIZE) return false; // impossible zig
            zig_center = &black_top_ziggurat_center;
            zig_leftside = &black_top_ziggurat_leftside;
            zig_rightside = &black_top_ziggurat_rightside;
        }
        else if (x == BOARD_SIZE-3) // bot-right side
        {
            if (y < 0 || y > BOARD_SIZE-3) return false; // impossible zig
            zig_center = &black_bot_ziggurat_center;
            zig_leftside = &black_bot_ziggurat_leftside;
            zig_rightside = &black_bot_ziggurat_rightside;
        }
        else return false; // invalid zig
    }
    else    // white
    {
        if (y == 2) // top-right side
        {
            if (x < 2 || x >= BOARD_SIZE) return false; // impossible zig
            zig_center = &white_top_ziggurat_center;
            zig_leftside = &white_top_ziggurat_leftside;
            zig_rightside = &white_top_ziggurat_rightside;
        }
        else if (y == BOARD_SIZE-3) // bot-left side
        {
            if (x < 0 || x > BOARD_SIZE-3) return false; // impossible zig
            zig_center = &white_bot_ziggurat_center;
            zig_leftside = &white_bot_ziggurat_leftside;
            zig_rightside = &white_bot_ziggurat_rightside;
        }
        else return false; // invalid zig
        
    }

    int enemy = get_enemy[player];

    // checking zig center
    for (int i=0; i<6; i++)
    {
        Board_XY zig_xy = (*zig_center)[i];
        int nx = x + zig_xy.x;
        int ny = y + zig_xy.y;
        if (board[ny][nx] == enemy)
        {
            if (can_meet_one_enemy)
            {
                can_meet_one_enemy = false;
            }
            else
            {
                return false;
            }
        }
    }

    // if the center is free, we can look at the sides
    


    // first, special board-edge cases



    // if can't check left side
    
    bool can_check_left_side = true;

    if (player == PLAYER_1_BLACK)   // black
    {
        if ((y == BOARD_SIZE-1 && x == 2)    ||    (y == BOARD_SIZE-3 && x == BOARD_SIZE-3))
            can_check_left_side = false;
    }
    else    // white
    {
        if ((y == BOARD_SIZE-3 && x == 0)    ||    (y == 2 && x == 2))
            can_check_left_side = false;
    }
    
    if (can_check_left_side == false)
    {
        // only check right side
        for (int i=0; i<3; i++)
        {
            Board_XY zig_xy = (*zig_leftside)[i];
            int nx = x + zig_xy.x;
            int ny = y + zig_xy.y;
            if (board[ny][nx] == enemy)
            {
                if (can_meet_one_enemy)
                {
                    can_meet_one_enemy = false;
                }
                else
                {
                    return false;
                }
            }
        }
        return true;
    }



    // if can't check right side
    
    bool can_check_right_side = true;

    if (player == PLAYER_1_BLACK)   // black
    {
        if ((y == 2 && x == 2)    ||    (y == 0 && x == BOARD_SIZE-3))
            can_check_right_side = false;
    }
    else    // white
    {
        if ((y == BOARD_SIZE-3 && x == BOARD_SIZE-3)    ||    (y == 2 && x == BOARD_SIZE-1))
            can_check_right_side = false;
    }
    
    if (can_check_right_side == false)
    {
        // only check right side
        for (int i=0; i<3; i++)
        {
            Board_XY zig_xy = (*zig_rightside)[i];
            int nx = x + zig_xy.x;
            int ny = y + zig_xy.y;
            if (board[ny][nx] == enemy)
            {
                if (can_meet_one_enemy)
                {
                    can_meet_one_enemy = false;
                }
                else
                {
                    return false;
                }
            }
        }
        return true;
    }


    // GENERAL CASE

    // check left side
    int enemies_leftside=0;
    for (int i=0; i<3; i++)
    {
        Board_XY zig_xy = (*zig_leftside)[i];
        int nx = x + zig_xy.x;
        int ny = y + zig_xy.y;
        if (board[ny][nx] == enemy)
        {
            enemies_leftside++;
            break;
        }
    }

    if (enemies_leftside == 0)
        return true; // left side is free!
    if (can_meet_one_enemy && enemies_leftside == 1)
        return true; // left side is free!

    // finally check the right side
    for (int i=0; i<3; i++)
    {
        Board_XY zig_xy = (*zig_rightside)[i];
        int nx = x + zig_xy.x;
        int ny = y + zig_xy.y;
        if (board[ny][nx] == enemy)
        {
            if (can_meet_one_enemy)
            {
                can_meet_one_enemy = false;
            }
            else
            {
                return false;
            }
        }
    }
    return true;

    
}


Player is_owned_by(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int x, int y) {
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

// considering next player to move. Breaking zero-sum principle ? Would not work with minimax ?
bool is_blocked_by_enemy_bridge(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], Player enemy, Player next_player, int x, int y, enum DIRECT_NEIGHBORS ni) {
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

bool is_connected_to_end_border(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int player, int x, int y, Player next_player) {
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
        if (x == BOARD_SIZE - 3) {  // zig
            if (y < BOARD_SIZE - 1) {
                if (is_free_ziggurat(board, x, y, player, next_player))
                {
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
        if (y == BOARD_SIZE - 3) {  // zig
            if (x < BOARD_SIZE - 2) {
                if (is_free_ziggurat(board, x, y, player, next_player))
                {
                    return true;
                }
            }
        }
    }


    return false;
}


static void copy_board(int dst[MAX_BOARD_SIZE][MAX_BOARD_SIZE],
                       int src[MAX_BOARD_SIZE][MAX_BOARD_SIZE])
{
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            dst[y][x] = src[y][x];
        }
    }
}

static void apply_move(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE],
                       int x, int y, Player player)
{
    board[y][x] = player;
}

// racine carrée entière approx
static int isqrt_int(int x)
{
    int r = 0;
    int bit = 1 << 30;
    while (bit > x) bit >>= 2;
    while (bit != 0) {
        int t = r + bit;
        if (x >= t) {
            x -= t;
            r = t + bit;
        }
        r >>= 1;
        bit >>= 2;
    }
    return r;
}

static int ucb1_score(int node_index, int parent_visits)
{
    MCTS_Node *n = &mcts_nodes[node_index];

    if (n->visits == 0) {
        // favorise les noeuds jamais visités
        return 1000000000;
    }

    // exploitation : valeur moyenne sur [0..1000]
    int mean_value = (1000 * n->total_value) / n->visits;

    // exploration : C * sqrt( parent_visits / visits )
    int ratio = (1000 * parent_visits) / n->visits;
    if (ratio < 0) ratio = 0;
    int explore_term = MCTS_EXPLORE_CONST * isqrt_int(ratio); // encore en [0.. ~C*sqrt(1000)]

    return mean_value + explore_term;
}

// tri insertion simple, vu qu’il y a peu d’éléments
static void sort_moves_by_score(MoveCandidate *moves, int count)
{
    for (int i = 1; i < count; i++) {
        MoveCandidate key = moves[i];
        int j = i - 1;
        while (j >= 0 && moves[j].score > key.score) {
            moves[j + 1] = moves[j];
            j--;
        }
        moves[j + 1] = key;
    }
}

static int generate_children(int node_index, int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE])
{
    MCTS_Node *node = &mcts_nodes[node_index];

    MoveCandidate candidates[BOARD_SIZE * BOARD_SIZE];
    int cand_count = 0;

    Player player = node->player_to_move;
    Player enemy  = get_enemy[player];

    // 1) collecter tous les coups possibles + les évaluer grossièrement
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            thinking_progress+=2;
            thinking_progress_max+=2;
            if (board[y][x] == 0) {
                apply_move(board, x, y, player);
                int lmtw_own = least_moves_to_win(board, player, enemy);
                // on peut aussi y ajouter une légère pénalité/bonus selon lmtw ennemi
                int lmtw_enemy = least_moves_to_win(board, enemy, enemy);
                int score = lmtw_own * 2 - lmtw_enemy; // pondération simple

                // revert move
                board[y][x] = 0;

                candidates[cand_count].x = x;
                candidates[cand_count].y = y;
                candidates[cand_count].score = score;
                cand_count++;
            }
        }
    }

    if (cand_count == 0) {
        node->first_child = -1;
        node->child_count = 0;
        return 0;
    }

    // 2) trier par score
    sort_moves_by_score(candidates, cand_count);

    // 3) créer les noeuds enfants jusqu'à MCTS_MAX_CHILDREN ou fin
    int start_index = mcts_node_count;
    node->first_child = start_index;
    node->child_count = 0;

    int max_children = cand_count;
    if (max_children > MCTS_MAX_CHILDREN) {
        max_children = MCTS_MAX_CHILDREN;
    }

    for (int i = 0; i < max_children && mcts_node_count < MCTS_MAX_NODES; i++) {
        MCTS_Node *child = &mcts_nodes[mcts_node_count];

        child->move_x = candidates[i].x;
        child->move_y = candidates[i].y;
        child->player_to_move = get_enemy[node->player_to_move]; // alternance
        child->parent = node_index;
        child->first_child = -1;
        child->child_count = 0;
        child->visits = 0;
        child->total_value = 0;

        node->child_count++;
        mcts_node_count++;
    }

    return node->child_count;
}


static void rebuild_board_from_root(int node_index,
                                    int base_board[MAX_BOARD_SIZE][MAX_BOARD_SIZE],
                                    int work_board[MAX_BOARD_SIZE][MAX_BOARD_SIZE],
                                    Player root_player)
{

    (void) root_player;
    copy_board(work_board, base_board);

    // remonter jusqu'à la racine
    int path[64];
    int path_len = 0;

    int current = node_index;
    while (current != 0 && current != -1) {
        path[path_len++] = current;
        current = mcts_nodes[current].parent;
    }

    // rejouer dans l'ordre racine -> feuille
    for (int i = path_len - 1; i >= 0; i--) {
        MCTS_Node *n = &mcts_nodes[path[i]];
        // le joueur qui a joué ce coup est l'ennemi de n->player_to_move
        Player mover = get_enemy[mcts_nodes[path[i]].player_to_move];
        apply_move(work_board, n->move_x, n->move_y, mover);
    }
}

static int random_playout(int node_index,
                          int base_board[MAX_BOARD_SIZE][MAX_BOARD_SIZE],
                          Player root_player)
{
    int board_copy[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    rebuild_board_from_root(node_index, base_board, board_copy, root_player);

    Player player = mcts_nodes[node_index].player_to_move;
    Player enemy  = get_enemy[player];

    int max_moves = 20; // playout très court

    for (int moves = 0; moves < max_moves; moves++) {

        thinking_progress++;
        // Vérification rapide toutes les 4 itérations
        if ((moves & 3) == 0) {
            int lmtw_root  = fast_lmtw(board_copy, root_player);
            int lmtw_other = fast_lmtw(board_copy, get_enemy[root_player]);

            if (lmtw_root == 0 && lmtw_other != 0)
                return 1000; // root gagne
            if (lmtw_other == 0 && lmtw_root != 0)
                return 0;    // root perd
        }

        // Liste des coups possibles
        int empty_cells[BOARD_SIZE * BOARD_SIZE][2];
        int empty_count = 0;

        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (board_copy[y][x] == 0) {
                    empty_cells[empty_count][0] = x;
                    empty_cells[empty_count][1] = y;
                    empty_count++;
                }
            }
        }

        if (empty_count == 0)
            break;

        // Coup aléatoire
        int r = qran_range(0, empty_count);
        int x = empty_cells[r][0];
        int y = empty_cells[r][1];

        apply_move(board_copy, x, y, player);

        // changer de joueur
        Player tmp = player;
        player = enemy;
        enemy = tmp;
    }

    // Évaluation finale rapide
    int final_root  = fast_lmtw(board_copy, root_player);
    int final_other = fast_lmtw(board_copy, get_enemy[root_player]);

    if (final_root < final_other) return 800;
    if (final_root > final_other) return 200;
    return 500;
}


static int mcts_select_leaf(int root_index)
{
    int current = root_index;

    while (1) {
        MCTS_Node *node = &mcts_nodes[current];

        if (node->child_count == 0) {
            // pas encore expandu
            return current;
        }

        // sélectionner l'enfant avec le meilleur UCB1
        int best_index = -1;
        int best_score = -2147483647;
        int parent_visits = node->visits;

        for (int i = 0; i < node->child_count; i++) {
            int child_index = node->first_child + i;
            int score = ucb1_score(child_index, parent_visits);
            if (score > best_score) {
                best_score = score;
                best_index = child_index;
            }
        }

        current = best_index;
    }
}

// renvoie l’index du noeud choisi pour la simulation
static int mcts_expand_if_possible(int leaf_index,
                                   int base_board[MAX_BOARD_SIZE][MAX_BOARD_SIZE],
                                   Player root_player)
{
    if (mcts_node_count > 100) {
        return leaf_index; // ne plus expandre
    }

    MCTS_Node *leaf = &mcts_nodes[leaf_index];

    if (leaf->visits == 0) {
        // pas encore évalué, pas besoin d’expansion, on va simuler dessus
        return leaf_index;
    }

    // on reconstruit le board pour ce leaf
        int work_board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
        rebuild_board_from_root(leaf_index, base_board, work_board, root_player);

        int created = generate_children(leaf_index, work_board);
        if (created == 0) {
            // pas d’enfants possibles, on reste sur la feuille
            return leaf_index;
        }

        // choisir un enfant au hasard pour la simulation
        int child_offset = qran_range(0, leaf->child_count);
        return leaf->first_child + child_offset;
}

static void mcts_backpropagate(int node_index, int value)
{
    int current = node_index;
    while (current != -1) {
        MCTS_Node *n = &mcts_nodes[current];
        n->visits++;
        n->total_value += value;
        current = n->parent;
    }
}

Board_XY mcts_find_next_move(int base_board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], Player ai_player)
{

    thinking_progress = 0;
    thinking_progress_max = MCTS_SIMULATIONS * 20;

    // initialiser la pool de noeuds
    mcts_node_count = 0;

    // créer la racine
    MCTS_Node *root = &mcts_nodes[0];
    root->move_x = -1;
    root->move_y = -1;
    root->player_to_move = ai_player;  // c’est à l’IA de jouer
    root->parent = -1;
    root->first_child = -1;
    root->child_count = 0;
    root->visits = 0;
    root->total_value = 0;

    mcts_node_count = 1;
    
    int work_board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    copy_board(work_board, base_board);
    generate_children(0, work_board);

    // simulations
    for (int sim = 0; sim < MCTS_SIMULATIONS; sim++) {

        // 1) sélection
        int leaf_idx = mcts_select_leaf(0);

        // 2) expansion
        int node_to_sim = mcts_expand_if_possible(leaf_idx, base_board, ai_player);

        // 3) simulation
        int value = random_playout(node_to_sim, base_board, ai_player);

        // 4) backprop
        mcts_backpropagate(node_to_sim, value);

        // optionnel: mettre à jour une animation (thinking_progress, etc.)
    }

    // choisir le meilleur coup : enfant de la racine avec le plus de visites
    if (root->child_count == 0) {
        // aucun coup ? bizarre, on renvoie (0,0) par défaut
        return new_board_xy(0, 0);
    }

    int best_child = -1;
    int best_visits = -1;
    for (int i = 0; i < root->child_count; i++) {
        int child_idx = root->first_child + i;
        MCTS_Node *c = &mcts_nodes[child_idx];
        if (c->visits > best_visits) {
            best_visits = c->visits;
            best_child = child_idx;
        }
    }

    MCTS_Node *choice = &mcts_nodes[best_child];
    return new_board_xy(choice->move_x, choice->move_y);
}

// Version ultra-rapide pour les playouts MCTS
// Pas de ziggurat, pas de next_player, pas de 0-1 BFS.
// Juste une estimation simple de distance de connexion.

#define FAST_INF  9999

int fast_lmtw(int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], Player player)
{
    static uint16_t dist[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    static uint8_t visited[MAX_BOARD_SIZE][MAX_BOARD_SIZE];

    // reset visited + dist
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            visited[y][x] = 0;
            dist[y][x] = FAST_INF;
        }
    }

    // queue statique
    int qx[BOARD_SIZE * BOARD_SIZE];
    int qy[BOARD_SIZE * BOARD_SIZE];
    int qh = 0, qt = 0;

    Player enemy = get_enemy[player];

    // Initialisation selon le joueur
    if (player == PLAYER_1_BLACK) {
        // bord gauche (x=0)
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (board[y][0] != enemy) {
                dist[y][0] = (board[y][0] == player) ? 0 : 1;
                visited[y][0] = 1;
                qx[qt] = 0;
                qy[qt] = y;
                qt++;
            }
        }
    } else {
        // bord haut (y=0)
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[0][x] != enemy) {
                dist[0][x] = (board[0][x] == player) ? 0 : 1;
                visited[0][x] = 1;
                qx[qt] = x;
                qy[qt] = 0;
                qt++;
            }
        }
    }

    // BFS simple
    while (qh < qt) {
        int x = qx[qh];
        int y = qy[qh];
        qh++;

        int d = dist[y][x];

        // condition d'arrivée
        if (player == PLAYER_1_BLACK) {
            if (x == BOARD_SIZE - 1)
                return d;
        } else {
            if (y == BOARD_SIZE - 1)
                return d;
        }

        // voisins directs (6 directions)
        static const int dx[6] = {-1, 0, -1, 1, 0, 1};
        static const int dy[6] = {-1, -1, 0, 0, 1, 1};

        for (int i = 0; i < 6; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if ((unsigned)nx >= (unsigned)BOARD_SIZE || (unsigned)ny >= (unsigned)BOARD_SIZE)
                continue;

            if (visited[ny][nx])
                continue;

            if (board[ny][nx] == enemy)
                continue; // mur

            int cost = (board[ny][nx] == player) ? 0 : 1;
            int nd = d + cost;

            if (nd < dist[ny][nx]) {
                dist[ny][nx] = nd;
                visited[ny][nx] = 1;
                qx[qt] = nx;
                qy[qt] = ny;
                qt++;
            }
        }
    }

    return FAST_INF;
}
