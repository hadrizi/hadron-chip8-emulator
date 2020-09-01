#include "Sound.h"

Sound::~Sound()
{
	SDL_CloseAudioDevice(device);
	SDL_FreeWAV(wav_buffer);
}

void Sound::load(const char* filename)
{
	SDL_LoadWAV(filename, &wav_spec, &wav_buffer, &wav_length);
	device = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
}

void Sound::play()
{
	SDL_QueueAudio(device, wav_buffer, wav_length);
	SDL_PauseAudioDevice(device, 0);
}