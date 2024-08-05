#pragma once

typedef enum MENU_TYPE
{
    MENU_PAUSE,
    MENU_P1WINS,
    MENU_P2WINS,
    MENU_YOUWIN,
    MENU_GAMEOVER
} MENU_TYPE;

extern MENU_TYPE current_menu_type;

void menu_loop();

void init_menu(MENU_TYPE type);