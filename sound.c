#include <stdio.h>
#include <stdint.h>

void emulate_sound(uint8_t* memory)
{
	// check if there is anything in 0x4000-0xffff
	int i = 0;
	for (i = 0x00004000; i <= 0x00004017; i++) {
		if (memory[i]) {
			fprintf(stdout,"Address %x on APU\n", memory[i]);
		}
	}
}