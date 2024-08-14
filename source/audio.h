#pragma once

#include <tonc.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

extern mm_sound_effect sfx_whoosh;
extern mm_sound_effect sfx_cursor;
extern mm_sound_effect sfx_put_stone;
extern mm_sound_effect sfx_bee;
extern mm_sound_effect sfx_beeloop;
extern mm_sfxhand sfx_beeloop_handle;
extern mm_sound_effect sfx_larva;
extern mm_sound_effect sfx_confirm;

void init_audio();

void play_music(int mod_id);

void play_sfx(mm_sound_effect* sfx);
