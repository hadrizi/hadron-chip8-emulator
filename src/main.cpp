#include <iostream>
#include <bitset>

#include <SDL.h>

#include "hadron8.h"

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Usage: hadron-chip8.exe [game_filename]\n\n");
		return 1;
	}

	hadron8 h8;

	bool running = true;

	if (!h8.load_game(argv[1]))
		return 1;
	 
	while (h8.get_exit() == 0)
	{
		/*
					Emulate hex keyboard

			Keypad                   Keyboard
			+-+-+-+-+                +-+-+-+-+
			|1|2|3|C|                |1|2|3|4|
			+-+-+-+-+                +-+-+-+-+
			|4|5|6|D|                |Q|W|E|R|
			+-+-+-+-+       =>       +-+-+-+-+
			|7|8|9|E|                |A|S|D|F|
			+-+-+-+-+                +-+-+-+-+
			|A|0|B|F|                |Z|X|C|V|
			+-+-+-+-+                +-+-+-+-+
		*/
		h8.emulate_keyboard();

		// Emulate one cycle
		h8.cycle();
	
		// If the draw flag is set, update the screen
		if (h8.get_draw() == 1)
			h8.draw_gfx();

		//h8.debug_render();
		//h8.debug_keys();
		//h8.debug_clock();
		//h8.debug_opcode();
	}
	
	return 0;
}