#pragma once

#define tile_mem_obj_tile  ((TILE*)MEM_VRAM_OBJ)   // same as tile_mem_obj[0]
#define tile8_mem_obj_tile ((TILE8*)MEM_VRAM_OBJ)  // same as tile8_mem_obj[0]

/////////////////
// BACKGROUNDS //
/////////////////

// set in grit command "-mp#"
// PALETTES (PAL RAM)     max 16
#define BGPAL_BOARD           0

#define BGPAL_TITLEBG         0
#define BGPAL_TITLEBEEKEEPER  1
#define BGPAL_TITLEHEXADVANCE 2
#define BGPAL_TITLEMENUTEXT   3

#define BGPAL_OPTIONS         1

// CBB (VRAM)           max 4
#define CBB_BOARD           0
#define CBB_BOARDBG         1

#define CBB_TITLEBG         0
#define CBB_TITLEBEEKEEPER  1
#define CBB_TITLEHEXADVANCE 2
#define CBB_TITLEMENUTEXT   3

#define CBB_OPTIONS         1

// SBB (VRAM)           max 32
#define SBB_BOARD           31      // screenblocks 1
#define SBB_BOARDBG         30      // screenblocks 1

#define SBB_TITLEBG         31      // screenblocks 1
#define SBB_TITLEBEEKEEPER  30
#define SBB_TITLEHEXADVANCE 29
#define SBB_TITLEMENUTEXT   28

#define SBB_OPTIONS         30

/////////////
// OBJECTS //
/////////////

// PALETTES (PAL RAM)   max 16
#define PAL_BEE             0
#define PAL_GHOST_STONE     1
#define PAL_STONESET_BLACK  2
#define PAL_STONESET_WHITE  3
#define PAL_LARVA           4
#define PAL_MENUS		    15

// SPRITES (OAM)        max 128
#define OAM_CURSOR          0
#define OAM_CURSOR_BS       1
#define OAM_CURSOR_FM       2

#define OAM_MENUS           1
#define OAM_BEE             2
#define OAM_LARVA           3
#define OAM_GHOST_STONE     4
#define OAM_STONESET_FIRSTBLACK  5  // 49 sprites assigned
#define OAM_STONESET_FIRSTWHITE  49 + OAM_STONESET_FIRSTBLACK // 49 more sprites assigned

// TILES (VRAM)         max 1024
#define TILE_BEE            0       // spritetiles 16
#define TILE_LARVA          16      // spritetiles 4

#define TILE_GHOST_STONE    64      // spritetiles 4
#define TILE_STONESET       68      // spritetiles 256

#define TILE_MENUS          512     // spritetiles 64
#define TILE_CURSOR_BS      1017    // spritetiles 1
#define TILE_CURSOR_FM      1018    // spritetiles 4
#define TILE_CURSOR	        1022    // spritetiles 2

