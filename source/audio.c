#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"
#include "options_loop.h"

mm_sound_effect sfx_cursor = {
    { SFX_CURSOR } ,			// id
    (int)(1.0f * (1<<10)),	// rate
    0,		// handle
    255,	// volume
    127,	// panning
};

mm_sound_effect sfx_put_stone = {
    { SFX_PUT_STONE } ,			// id
    (int)(1.0f * (1<<10)),	// rate
    0,		// handle
    255,	// volume
    127,	// panning
};

mm_sound_effect sfx_bee = {
    { SFX_BEE } ,			// id
    (int)(1.0f * (1<<10)),	// rate
    0,		// handle
    255,	// volume
    127,	// panning
};

mm_sound_effect sfx_beeloop = {
    { SFX_BEELOOP } ,			// id
    (int)(1.0f * (1<<10)),	// rate
    0,		// handle
    255,	// volume
    127,	// panning
};

mm_sfxhand sfx_beeloop_handle = 0;

mm_sound_effect sfx_larva = {
    { SFX_LARVA } ,			// id
    (int)(1.0f * (1<<10)),	// rate
    0,		// handle
    255,	// volume
    127,	// panning
};

mm_sound_effect sfx_confirm = {
    // { SFX_CONFIRM } ,			// id
    { SFX_CURSOR } ,			// id
    (int)(1.0f * (1<<10)),	// rate
    0,		// handle
    255,	// volume
    127,	// panning
};


void init_audio()
{

    // moved into main
	// irq_init(NULL);
	// irq_add(II_VBLANK, mmVBlank);
    // irq_enable(II_VBLANK);

	// initialise maxmod with soundbank and 8 channels
    mmInitDefault( (mm_addr)soundbank_bin, 8 );
}

void play_music(int mod_id)
{
    if (option_music)
	    mmStart(mod_id, MM_PLAY_LOOP);
}

void play_sfx(mm_sound_effect* sfx)
{
    if (option_sound)
        mmEffectEx(sfx);
}