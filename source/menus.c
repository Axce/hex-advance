#include <tonc.h>
#include "menus.h"
#include "toolbox.h"
#include "graphics.h"
#include "mem_management.h"

MENU_TYPE current_menu_type;

void init_menu(MENU_TYPE type)
{
    current_menu_type = type;
    game_state = MENUING;
    
    GRIT_CPY(&pal_obj_bank[PAL_MENUS], menu_palettePal);

    switch (type)
    {
        case MENU_PAUSE:
            GRIT_CPY(&tile_mem_obj_tile[TILE_MENUS], menu_pauseTiles);
            break;
        case MENU_P1WINS:
            GRIT_CPY(&tile_mem_obj_tile[TILE_MENUS], menu_p1winsTiles);
            break;
        case MENU_P2WINS:
            GRIT_CPY(&tile_mem_obj_tile[TILE_MENUS], menu_p2winsTiles);
            break;
        case MENU_YOUWIN:
            GRIT_CPY(&tile_mem_obj_tile[TILE_MENUS], menu_youwinTiles);
            break;
        case MENU_GAMEOVER:
            GRIT_CPY(&tile_mem_obj_tile[TILE_MENUS], menu_gameoverTiles);
            break;
        default:
            return;
    }

    obj_set_attr(&obj_buffer[OAM_MENUS],
        ATTR0_SQUARE,
        ATTR1_SIZE_64x64,
        ATTR2_PALBANK(PAL_MENUS) | ATTR2_ID(TILE_MENUS));
    obj_set_pos(&obj_buffer[OAM_MENUS], 2, 2);
    obj_unhide(&obj_buffer[OAM_MENUS], DCNT_MODE0);

}

void menu_loop()
{

}