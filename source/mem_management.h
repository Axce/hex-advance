#pragma once

#define tile_mem_obj_tile  ((TILE*)MEM_VRAM_OBJ)   // same as tile_mem_obj[0]
#define tile8_mem_obj_tile ((TILE8*)MEM_VRAM_OBJ)  // same as tile8_mem_obj[0]

/////////////////
// BACKGROUNDS //
/////////////////

// PALETTES (PAL RAM)
#define BGPAL_BOARD         0

// CBB (VRAM)
#define CBB_BOARD           0

// SBB (VRAM)
#define SBB_BOARD           30      // screenblocks 2

/////////////
// OBJECTS //
/////////////

// PALETTES (PAL RAM)
#define PAL_BEE             0
#define PAL_GHOST_STONE     1
#define PAL_STONESET_BLACK  2
#define PAL_STONESET_WHITE  3
#define PAL_LARVA           4
#define PAL_MENUS		    15

// SPRITES (OAM)
#define OAM_TITLECURSOR     0
#define OAM_BEE             0
#define OAM_GHOST_STONE     1
#define OAM_STONESET_FIRSTBLACK  2  // 36 sprites assigned
#define OAM_STONESET_FIRSTWHITE  36 + OAM_STONESET_FIRSTBLACK // 36 more sprites assigned
#define OAM_MENUS           74
#define OAM_LARVA           75

// TILES (VRAM)
#define TILE_BEE            0       // spritetiles 16
#define TILE_LARVA          16      // spritetiles 4

#define TILE_GHOST_STONE    64      // spritetiles 4
#define TILE_STONESET       68      // spritetiles 256

#define TILE_TITLECURSOR	512     // spritetiles 2
#define TILE_MENUS          512     // spritetiles 64


// useless, already oam_init
// inline void clear_oam()
// {
// 	memset32(obj_mem, 0, sizeof(OBJ_ATTR)*OBJ_COUNT);
//     oam_init(obj_mem, OBJ_COUNT);
// }