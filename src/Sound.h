#pragma once
#include <iostream>

#include <SDL.h>

class Sound
{
public:
    ~Sound();
    void load(const char*);
    void play();

private:
    SDL_AudioSpec wav_spec;
    Uint32 wav_length;
    Uint8* wav_buffer = nullptr;
    SDL_AudioDeviceID device;
};