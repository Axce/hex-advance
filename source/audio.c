#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

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
    // irqInit();
    // irqSet( IRQ_VBLANK, mmVBlank );
	// irqEnable(IRQ_VBLANK);
	irq_init(NULL);
	irq_add(II_VBLANK, mmVBlank);
    irq_enable(II_VBLANK);

	// initialise maxmod with soundbank and 8 channels
    mmInitDefault( (mm_addr)soundbank_bin, 8 );

	// Start playing module
	//mmStart( MOD_FLATOUTLIES, MM_PLAY_LOOP );

}