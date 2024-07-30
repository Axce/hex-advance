#pragma once

typedef struct
{
    int x;
    int y;
} Screen_XY;

typedef struct
{
    int x;
    int y;
} Board_XY;

bool is_stone_in_board(Screen_XY xy);

Board_XY to_board_xy(Screen_XY xy);

Screen_XY to_screen_xy(Board_XY xy);

Screen_XY get_sprite_position(OBJ_ATTR* sprite);
