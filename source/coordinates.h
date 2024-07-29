#ifndef COORDINATES_H
#define COORDINATES_H

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

extern int board[11][11];

bool is_stone_in_board(Board_XY xy);

Board_XY to_board_xy(Screen_XY xy);

Screen_XY to_screen_xy(Board_XY xy);

#endif  /* !COORDINATES_H */