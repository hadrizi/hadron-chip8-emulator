#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <ctime>

#include <SDL.h>

#include "Sound.h"

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
	const char* beep_filename;

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

	Sound beep_sound;
private:
	void disp_clear();
	void reg_dump(int);
	void reg_load(int);
	void beep();

	inline void op_0000() { (this->*op_0000_table[(opcode & 0x000F) >> 0])(); }
	inline void op_8000() { (this->*op_8000_table[(opcode & 0x000F) >> 0])(); }
	inline void op_E000() { (this->*op_E000_table[(opcode & 0x000F) >> 0])(); }
	inline void op_F000() { (this->*op_F000_table[(opcode & 0x000F) >> 0])(); }
	inline void op_FX05() { (this->*op_FX05_table[(opcode & 0x00F0) >> 4])(); }

	// OPCODES
	///////////////////////////////////////////////////////////////
	inline void op_NULL() {};

	void op_00E0(); void op_00EE(); void op_1NNN(); void op_2NNN(); 
	void op_3XNN(); void op_4XNN(); void op_5XY0(); void op_6XNN(); 
	void op_7XNN(); void op_8XY0(); void op_8XY1(); void op_8XY2();
	void op_8XY3(); void op_8XY4(); void op_8XY5(); void op_8XY6();
	void op_8XY7(); void op_8XYE(); void op_9XY0(); void op_ANNN(); 
	void op_BNNN(); void op_CXNN(); void op_DXYN(); void op_EX9E();
	void op_EXA1(); void op_FX07(); void op_FX0A(); void op_FX15();
	void op_FX18(); void op_FX1E(); void op_FX29(); void op_FX33();
	void op_FX55(); void op_FX65();
	///////////////////////////////////////////////////////////////
	
	typedef void (hadron8::*op_XXXX)();

	static op_XXXX opcodes[16];
	static op_XXXX op_0000_table[16];
	static op_XXXX op_8000_table[16];
	static op_XXXX op_E000_table[16];
	static op_XXXX op_F000_table[16];
	static op_XXXX op_FX05_table[16];

	inline void inc_pc() { pc += 2; }
	inline void dec_delay() { --delay_timer; }
	inline void dec_sound() { --sound_timer; }
};

