#include <stdio.h>
#include <stdint.h>

void emulate_graphics(uint8_t* memory)
{
	int i = 0;
	for (i = 0x00002000; i <= 0x00003FFF; i++) {
		if (memory[i]) {
			fprintf(stdout,"Address %x on PPU\n", memory[i]);
		}
	}
}