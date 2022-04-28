/*
 * owl_sound.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include "SDL.h"

#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"

#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

#include "owl_sound.h"
#include "owl_table.h"

#define OWL_SOUND_NONE 0
#define OWL_SOUND_WAV 1
#define OWL_SOUND_FLAC 2
#define OWL_SOUND_MP3 3

typedef struct owl_Sound {
  SDL_AudioSpec spec;
  owl_Audio audio;
  u32 type;
  u32 size;
  u8 *buffer;
} owl_Sound;

static owl_Table *sounds = NULL;

static owl_Sound *owl_loadWAV(const char *filename) {
  owl_Sound *sound;
  SDL_AudioSpec spec;
  u8 *wav;
  u32 size;

  if (!SDL_LoadWAV(filename, &spec, &wav, &size))
    return NULL;

  sound = (owl_Sound *)calloc(1, sizeof(owl_Sound));

  if (!sound) {
    SDL_FreeWAV(wav);
    return NULL;
  }

  sound->type = OWL_SOUND_WAV;
  sound->spec = spec;
  sound->buffer = wav;
  sound->size = size;

  return sound;
}

static owl_Sound *owl_loadFlac(const char *filename) {
  owl_Sound *sound;
  u32 channels, sample_rate;
  u64 num_samples;
  s32 *samples;

  samples = drflac_open_file_and_read_pcm_frames_s32(
      filename, &channels, &sample_rate, &num_samples, NULL);

  if (!samples)
    return NULL;

  sound = (owl_Sound *)calloc(1, sizeof(owl_Sound));

  if (!sound) {
    drflac_free(samples, NULL);
    return NULL;
  }

  sound->type = OWL_SOUND_FLAC;
  sound->spec.freq = sample_rate;
  sound->spec.channels = channels;
  sound->spec.format = AUDIO_S32SYS;
  sound->spec.samples = 4096;
  sound->buffer = (u8 *)samples;
  sound->size = (u32)(num_samples * channels * sizeof(s32));

  return sound;
}

static owl_Sound *owl_loadMP3(const char *filename) {
  owl_Sound *sound;
  drmp3_config mp3;
  u64 num_samples;
  s16 *samples;

  samples = drmp3_open_file_and_read_pcm_frames_s16(filename, &mp3,
                                                    &num_samples, NULL);
  if (!samples)
    return NULL;

  sound = (owl_Sound *)calloc(1, sizeof(owl_Sound));

  if (!sound) {
    drmp3_free(samples, NULL);
    return NULL;
  }

  sound->type = OWL_SOUND_MP3;
  sound->spec.freq = mp3.sampleRate;
  sound->spec.channels = mp3.channels;
  sound->spec.format = AUDIO_S16SYS;
  sound->spec.samples = 4096;
  sound->buffer = (u8 *)samples;
  sound->size = (u32)(num_samples * mp3.channels * sizeof(s16));

  return sound;
}

static owl_Sound *owl_sound(const char *filename) {
  owl_Sound *sound;

  sound = owl_loadWAV(filename);

  if (sound)
    return sound;

  sound = owl_loadFlac(filename);

  if (sound)
    return sound;

  sound = owl_loadMP3(filename);

  if (sound)
    return sound;

  return NULL;
}

static void owl_freeSound(owl_Sound *sound) {
  owl_clearAudio(sound->audio);
  owl_closeAudio(sound->audio);

  switch (sound->type) {
  case OWL_SOUND_WAV:
    SDL_FreeWAV(sound->buffer);
    break;
  case OWL_SOUND_FLAC:
    drflac_free(sound->buffer, NULL);
    break;
  case OWL_SOUND_MP3:
    drmp3_free(sound->buffer, NULL);
    break;
  }

  free(sound);
}

static owl_Audio owl_openAudio(const SDL_AudioSpec *spec) {
  owl_Audio audio = SDL_OpenAudioDevice(NULL, 0, spec, NULL, 0);

  if (audio)
    SDL_PauseAudioDevice(audio, 0);

  return audio;
}

bool owl_soundInit(void) {
  if (!sounds)
    sounds = owl_table();
  return sounds != NULL;
}

void owl_soundQuit(void) {
  if (sounds) {
    owl_freeTable(sounds, (owl_Dtor)owl_freeSound);
    sounds = NULL;
  }
}

owl_Audio owl_audio(s32 freq, u8 format, u8 channels, u16 samples) {
  SDL_AudioSpec spec = {0};

  spec.freq = freq;
  spec.channels = channels;
  spec.samples = samples;

  switch (format) {
  case OWL_AUDIO_U8:
    spec.format = AUDIO_U8;
    break;
  case OWL_AUDIO_S8:
    spec.format = AUDIO_S8;
    break;
  case OWL_AUDIO_U16:
    spec.format = AUDIO_U16SYS;
    break;
  case OWL_AUDIO_S16:
    spec.format = AUDIO_S16SYS;
    break;
  case OWL_AUDIO_S32:
    spec.format = AUDIO_S32SYS;
    break;
  case OWL_AUDIO_F32:
    spec.format = AUDIO_F32SYS;
    break;
  default:
    return 0;
  }

  return owl_openAudio(&spec);
}

void owl_closeAudio(owl_Audio audio) {
  if (!audio)
    return;

  SDL_CloseAudioDevice(audio);
}

void owl_clearAudio(owl_Audio audio) {
  if (!audio)
    return;

  SDL_ClearQueuedAudio(audio);
}

void owl_playAudio(owl_Audio audio, bool onoff) {
  if (!audio)
    return;

  SDL_PauseAudioDevice(audio, !onoff);
}

bool owl_audioStream(owl_Audio audio, const void *buffer, s32 len) {
  if (!audio)
    return false;

  return 0 == SDL_QueueAudio(audio, buffer, len);
}

u32 owl_audioBuffered(owl_Audio audio) {
  if (!audio)
    return 0;

  return SDL_GetQueuedAudioSize(audio);
}

bool owl_loadSound(const char *name, const char *filename) {
  owl_Sound *sound = (owl_Sound *)owl_getTable(sounds, name);

  if (sound)
    return true;

  if (!filename)
    return false;

  sound = owl_sound(filename);

  if (!sound)
    return false;

  owl_setTable(sounds, name, sound);
  return true;
}

bool owl_playing(const char *name) {
  owl_Sound *sound = (owl_Sound *)owl_getTable(sounds, name);

  if (!sound)
    return false;

  return owl_audioBuffered(sound->audio) > 0;
}

bool owl_play(const char *name) {
  owl_Sound *sound = (owl_Sound *)owl_getTable(sounds, name);

  if (!sound)
    return false;

  if (!sound->audio)
    sound->audio = owl_openAudio(&sound->spec);

  return owl_audioStream(sound->audio, sound->buffer, sound->size);
}

bool owl_stop(const char *name) {
  owl_Sound *sound = (owl_Sound *)owl_getTable(sounds, name);

  if (!sound)
    return false;

  owl_clearAudio(sound->audio);
  return true;
}

bool owl_pause(const char *name) {
  owl_Sound *sound = (owl_Sound *)owl_getTable(sounds, name);

  if (!sound)
    return false;

  owl_playAudio(sound->audio, false);
  return true;
}

bool owl_resume(const char *name) {
  owl_Sound *sound = (owl_Sound *)owl_getTable(sounds, name);

  if (!sound)
    return false;

  owl_playAudio(sound->audio, true);
  return true;
}
