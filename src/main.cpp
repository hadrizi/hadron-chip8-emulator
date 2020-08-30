#include <iostream>
#include <bitset>

#include <SDL.h>

#include "hadron8.h"

int main(int, char**)
{
	hadron8 h8;
	
	/*
	{
		{
			h8.V[0] = 0;
			h8.V[1] = 0;

			h8.memory[0x200] = 0xD0;
			h8.memory[0x200 + 0x1] = 0x15;

			h8.memory[81] = 0x80;

			h8.I = 0;

			h8.cycle();
			if (h8.get_draw() == 1)
				h8.draw_gfx();
		}

		{
			h8.pc = 0x200;
			h8.V[0] = 6;
			h8.V[1] = 0;

			h8.memory[0x200] = 0xD0;
			h8.memory[0x200 + 0x1] = 0x15;

			h8.memory[81] = 0x80;

			h8.I = 5;

			h8.cycle();
			if (h8.get_draw() == 1)
				h8.draw_gfx();
		}

		{
			h8.pc = 0x200;
			h8.V[0] = 12;
			h8.V[1] = 0;

			h8.memory[0x200] = 0xD0;
			h8.memory[0x200 + 0x1] = 0x15;

			h8.memory[81] = 0x80;

			h8.I = 10;

			h8.cycle();
			if (h8.get_draw() == 1)
				h8.draw_gfx();
		}

		{
			h8.pc = 0x200;
			h8.V[0] = 18;
			h8.V[1] = 0;

			h8.memory[0x200] = 0xD0;
			h8.memory[0x200 + 0x1] = 0x15;

			h8.memory[81] = 0x80;

			h8.I = 15;

			h8.cycle();
			if (h8.get_draw() == 1)
				h8.draw_gfx();
		}


		SDL_Delay(20000);
	}
	*/

	bool running = true;

	if (!h8.load_game("C:\\personal\\8bitgames\\TANK"))
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