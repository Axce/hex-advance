#pragma once

extern int title_bg_hofs;
extern int title_bg_vofs;
extern int title_beekeeper_vofs_n;

void init_title_screen();

void title_screen_loop();

void init_tutorial_loop();

void tutorial_loop();

void play_transition_in();
void play_transition_out();
void disp_transition_sprites(int i);

// for a max 64x64 sprite
INLINE void obj_set_pos_safe(OBJ_ATTR *obj, int x, int y)
{
    if ( x >= -64 && x < 240 && y >= -64 && y < 160 )
        obj_set_pos(obj, x, y);
    else
        obj_set_pos(obj, -64, -64);
}