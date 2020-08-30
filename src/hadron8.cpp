#include "hadron8.h"

// 0x000 - 0x1FF - Chip 8 interpreter (contains font set in emu)
// 0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
// 0x200 - 0xFFF - Program ROM and work RAM

hadron8::hadron8()
	: pc(0x200), opcode(0), I(0), sp(0), delay_timer(0), sound_timer(0), inc(1), draw(1), exit_emulation(0),
		renderer(nullptr), window(nullptr), texture(nullptr), pixels(nullptr),
		V{ 0 }, stack{ 0 }, gfx{ 0 }, memory{ 0 }, key{ 0 }
{
	// Clear display
	for (int i = 0; i < 2048; ++i)
		gfx[i] = 0;

	// Clear stack
	for (int i = 0; i < 16; ++i)
		stack[i] = 0;

	for (int i = 0; i < 16; ++i)
		key[i] = V[i] = 0;

	// Clear memory
	for (int i = 0; i < 4096; ++i)
		memory[i] = 0;

	/*
					EXAMPLE

		DEC   HEX    BIN         RESULT
		240   0xF0   1111 0000    ****     
		144   0x90   1001 0000    *  *      
		144   0x90   1001 0000    *  *      
		144   0x90   1001 0000    *  *      
		240   0xF0   1111 0000    ****
		
		240   0xF0   1111 0000    ****
		 16   0x10   0001 0000       *
		 32   0x20   0010 0000      *
		 64   0x40   0100 0000     *
		 64   0x40   0100 0000     *

		240   0xF0   1111 0000    ****
		 16   0x10   0001 0000       *
		 32   0xF0   0010 0000    ****
		 64   0x10   0100 0000    *    
		 64   0xF0   0100 0000    ****
	*/
	uint8_t chip8_fontset[80] =
	{
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	window = SDL_CreateWindow("hadron_chip8_emu", 100, 100, 640, 320, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		SDL_DestroyWindow(window);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 640, 320);
	if (texture == nullptr) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	pixels = new Uint32[640 * 320];
	memset(pixels, 0, 640 * 320 * sizeof(Uint32));
}

hadron8::~hadron8()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void hadron8::disp_clear()
{
	for (int i = 0; i < 64 * 32; ++i)
		gfx[i] = 0;
	draw = 1;
}

void hadron8::reg_dump(int x)
{
	for (int i = 0; i <= x; ++i)
		memory[I + i] = V[i];
}

void hadron8::reg_load(int x)
{
	for (int i = 0; i <= x; ++i)
		V[i] = memory[I + i];
}

void hadron8::emulate_keyboard()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			exit_emulation = 1;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.scancode)
			{
			case SDL_SCANCODE_1:
				key[0x1] = 1;
				break;
			case SDL_SCANCODE_2:
				key[0x2] = 1;
				break;
			case SDL_SCANCODE_3:
				key[0x3] = 1;
				break;
			case SDL_SCANCODE_4:
				key[0xC] = 1;
				break;

			case SDL_SCANCODE_Q:
				key[0x4] = 1;
				break;
			case SDL_SCANCODE_W:
				key[0x5] = 1;
				break;
			case SDL_SCANCODE_E:
				key[0x6] = 1;
				break;
			case SDL_SCANCODE_R:
				key[0xD] = 1;
				break;

			case SDL_SCANCODE_A:
				key[0x7] = 1;
				break;
			case SDL_SCANCODE_S:
				key[0x8] = 1;
				break;
			case SDL_SCANCODE_D:
				key[0x9] = 1;
				break;
			case SDL_SCANCODE_F:
				key[0xE] = 1;
				break;

			case SDL_SCANCODE_Z:
				key[0xA] = 1;
				break;
			case SDL_SCANCODE_X:
				key[0x0] = 1;
				break;
			case SDL_SCANCODE_C:
				key[0xB] = 1;
				break;
			case SDL_SCANCODE_V:
				key[0xF] = 1;
				break;

			default:
				puts("Incorrect key pressed");
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.scancode)
			{
			case SDL_SCANCODE_1:
				key[0x1] = 0;
				break;
			case SDL_SCANCODE_2:
				key[0x2] = 0;
				break;
			case SDL_SCANCODE_3:
				key[0x3] = 0;
				break;
			case SDL_SCANCODE_4:
				key[0xC] = 0;
				break;

			case SDL_SCANCODE_Q:
				key[0x4] = 0;
				break;
			case SDL_SCANCODE_W:
				key[0x5] = 0;
				break;
			case SDL_SCANCODE_E:
				key[0x6] = 0;
				break;
			case SDL_SCANCODE_R:
				key[0xD] = 0;
				break;

			case SDL_SCANCODE_A:
				key[0x7] = 0;
				break;
			case SDL_SCANCODE_S:
				key[0x8] = 0;
				break;
			case SDL_SCANCODE_D:
				key[0x9] = 0;
				break;
			case SDL_SCANCODE_F:
				key[0xE] = 0;
				break;

			case SDL_SCANCODE_Z:
				key[0xA] = 0;
				break;
			case SDL_SCANCODE_X:
				key[0x0] = 0;
				break;
			case SDL_SCANCODE_C:
				key[0xB] = 0;
				break;
			case SDL_SCANCODE_V:
				key[0xF] = 0;
				break;

			default:
				puts("Incorrect key released");
			}
			break;
		default:
			break;
		}
	} // end of message processing
}

bool hadron8::load_game(const char* game_file)
{
	printf("Loading: %s\n", game_file);

	FILE* game = fopen(game_file, "rb");
	if (game == NULL)
	{
		fputs("File error", stderr);
		return false;
	}

	fseek(game, 0, SEEK_END);
	long lSize = ftell(game);
	rewind(game);
	printf("Game size: %d\n", (int)lSize);

	// Allocate memory to contain the whole file
	int8_t* buffer = (int8_t*)malloc(sizeof(int8_t) * lSize);
	if (buffer == NULL)
	{
		fputs("Memory error", stderr);
		return false;
	}

	// Copy the file into the buffer
	size_t result = fread(buffer, 1, lSize, game);
	if (result != lSize)
	{
		fputs("Reading error", stderr);
		return false;
	}

	// Copy buffer to Chip8 memory
	if ((4096 - 512) > lSize)
	{
		for (int i = 0; i < lSize; ++i)
			memory[i + 512] = buffer[i];
	}
	else
		printf("Error: ROM too big for memory");

	// Close file, free buffer
	fclose(game);
	free(buffer);

	return true;
}

void hadron8::cycle()
{
	opcode = memory[pc] << 8 | memory[pc + 1];

	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x00FF)
		{
		// 0x00E0 Clears the screen
		case 0x00E0:
			disp_clear();
			break;
		// 0x00EE Returns from soubroutine
		case 0x00EE:
			--sp;
			pc = stack[sp];
			break;
		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
		}
		break;
	case 0x1000:
		pc = opcode & 0x0FFF;
		inc = 0;
		break;
	case 0x2000:
		stack[sp] = pc;
		++sp;
		pc = opcode & 0x0FFF;
		inc = 0;
		break;
	case 0x3000:
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			inc_pc();
		break;
	case 0x4000:
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			inc_pc();
		break;
	case 0x5000:
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
			inc_pc();
		break;
	case 0x6000:
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		break;
	case 0x7000:
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		break;
	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000:
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0001:
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0002:
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0003:
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0004:
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 1;
			else
				V[0xF] = 0;
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0005:
			if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
				V[0xF] = 0;
			else
				V[0xF] = 1;
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0006:
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
			V[(opcode & 0x0F00) >> 8] >>= 1;
			break;
		case 0x0007:
			if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
				V[0xF] = 0;
			else
				V[0xF] = 1;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			break;
		case 0x000E:
			V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
			V[(opcode & 0x0F00) >> 8] <<= 1;
			break;
		default:
			printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
		}
		break;
	case 0x9000:
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
			inc_pc();
		break;
	case 0xA000:
		I = opcode & 0x0FFF;
		break;
	case 0xB000:
		pc = V[0x0] + (opcode & 0x0FFF);
		inc = 0;
		break;
	case 0xC000:
		srand(time(NULL));
		V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
		break;
	case 0xD000:
	/*
		Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory
		location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when
		the sprite is drawn, and to 0 if that doesn’t happen
		
		draw(Vx,Vy,N)

				EXAMPLE

		memory[I]     = 0x3C;
		memory[I + 1] = 0xC3;
		memory[I + 2] = 0xFF;
		
					^
					|

		HEX    BIN        Sprite
		0x3C   00111100     ****
		0xC3   11000011   **    **
		0xFF   11111111   ********


	*/
	{
		uint16_t x = V[(opcode & 0x0F00) >> 8];
		uint16_t y = V[(opcode & 0x00F0) >> 4];
		uint16_t height = opcode & 0x000F;
		uint16_t pattern;

		V[0xF] = 0;
		for (int y_offset = 0; y_offset < height; ++y_offset)
		{
			pattern = memory[I + y_offset];
			for (int x_offset = 0; x_offset < 8; ++x_offset)
			{
				if ((pattern & (0x80 >> x_offset)) != 0)
				{
					if (gfx[(x + x_offset + ((y + y_offset) * 64))] == 1)
						V[0xF] = 1;
					gfx[x + x_offset + ((y + y_offset) * 64)] ^= 1;
				}
			}
		}

		draw = 1;
	}
		break;
	case 0xE000:
		switch (opcode & 0x00FF)
		{
		case 0x009E:
			if (key[V[(opcode & 0x0F00) >> 8]] != 0)
				inc_pc();
			break;
		case 0x00A1:
			if (key[V[(opcode & 0x0F00) >> 8]] == 0)
				inc_pc();
			break;
		default:
			printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
		}
		break;
	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007:
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			break;
		case 0x000A:
			// A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
			// Vx = get_key()
		{
			bool key_press = false;
			for (int i = 0; i < 16; ++i)
			{
				if (key[i] != 0)
				{
					V[(opcode & 0x0F00) >> 8] = i;
					key_press = true;
				}
			}

			if (!key_press)
				return;
		}
			break;
		case 0x0015:
			delay_timer = V[(opcode & 0x0F00) >> 8];
			break;
		case 0x0018:
			sound_timer = V[(opcode & 0x0F00) >> 8];
			break;
		case 0x001E:
			if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
				V[0xF] = 1;
			else
				V[0xF] = 0;
			I += V[(opcode & 0x0F00) >> 8];
			break;
		case 0x0029:
			// Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
			// I=sprite_addr[Vx]
			I = V[(opcode & 0x0F00) >> 8] * 0x5;
			break;
		case 0x0033:
			memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
			memory[I + 1] =(V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;
			break;
		case 0x0055:
			reg_dump((opcode & 0x0F00) >> 8);
			I += ((opcode & 0x0F00) >> 8) + 1;
			break;
		case 0x0065:
			reg_load((opcode & 0x0F00) >> 8);
			I += ((opcode & 0x0F00) >> 8) + 1;
			break;
		default:
			printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
		}
		break;

	default:
		printf("Unknown opcode: 0x%X\n", opcode);
	}

	if (delay_timer > 0)
		dec_delay();

	if (sound_timer > 0)
	{
		if (sound_timer == 1)
			printf("BEEP!\n");
		dec_sound();
	}

	if (inc == 1)
		inc_pc();
	else
		inc = 1;
}

void hadron8::draw_gfx()
{
	SDL_UpdateTexture(texture, NULL, pixels, 640 * sizeof(Uint32));

	//SDL_RenderClear(renderer);
	for (int x = 0; x < 64; ++x)
	{
		for (int y = 0; y < 32; ++y)
		{
			Uint8 col = gfx[x + y * 64] == 0 ? 0x00 : 0xff;

			int _x = x * 10;
			int _y = y * 10;


			for (int x_offset = 0; x_offset < 10; ++x_offset)
			{
				for (int y_offset = 0; y_offset < 10; ++y_offset)
					pixels[_x + x_offset + ((_y + y_offset) * 640)] = col;
			}
		}
	}

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	draw = 0;
}

void hadron8::debug_render()
{
	for (int y = 0; y < 32; ++y)
	{
		for (int x = 0; x < 64; ++x)
		{
			if (gfx[(y * 64) + x] == 0)
				printf("O");
			else
				printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}

void hadron8::debug_keys()
{
	for (int i = 0; i < 16; i++)
	{
		if(key[i] == 1)
			printf("%d key is pressed\n", i);
	}
}

void hadron8::debug_clock()
{
	printf("Delay Clock is %d\n", delay_timer);
	printf("Sound Clock is %d\n", sound_timer);
}

void hadron8::debug_opcode()
{
	printf("Opcode is 0x%X\n", opcode);
}
