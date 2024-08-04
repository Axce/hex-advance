#include "PlayerSprite.h"
#include "graphics.h"
#include "game.h"
#include <string.h>
#include "coordinates.h"
#include "cpu_player.h"

void restart_minigame()
{
    memset(board, 0, sizeof(board));

    board[5][5] = LARVA;

    init_stones_sprites();
    current_player = PLAYER_1_BLACK;
    switch_player_graphics();
    winner = 0;
    obj_hide(&obj_buffer[74]);          //hide win text
    obj_unhide(bee.obj, DCNT_MODE0);
    update_bee_sprite();

    game_state = IN_GAME;
}

void larva_play()
{

}