#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define BUFFER_SIZE 3583

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <ctime>

#include <SDL.h>

// 0x000 - 0x1FF - Chip 8 interpreter (contains font set in emu)
// 0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
// 0x200 - 0xFFF - Program ROM and work RAM

class hadron8
{
public:
	hadron8();
	~hadron8();

	bool load_game(const char*);
	void cycle();
	void draw_gfx();
	void emulate_keyboard();

	void debug_render();
	void debug_keys();
	void debug_clock();
	void debug_opcode();

	inline uint8_t get_draw() const { return draw; }
	inline uint8_t get_exit() const { return exit_emulation; }
private:
	uint16_t stack[16];
	uint16_t sp;
	
	uint16_t opcode;
	uint8_t memory[4 * 1024];
	uint16_t I;
	uint16_t pc;
	uint8_t V[16];
	uint8_t exit_emulation;
	uint8_t inc;
	uint8_t draw;
	uint8_t gfx[64 * 32];

	uint8_t delay_timer;
	uint8_t sound_timer;

	uint8_t key[16];

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	Uint32* pixels;
private:
	void disp_clear();
	void reg_dump(int);
	void reg_load(int);

	inline void inc_pc() { pc += 2; }
	inline void dec_delay() { --delay_timer; }
	inline void dec_sound() { --sound_timer; }
};

