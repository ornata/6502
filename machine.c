#include <stdio.h>
#include <stdint.h>
#include "machine.h"

#define RAM 0
#define PRGROM 1
#define CHRROM 2
#define SRAM 3
#define REGISTER 4
#define EXPANSION_ROM 5


/* Given an address, decide where we should look */
char map_mem(uint16_t address)
{

	if (address < 0x2000 && address >= 0x0000) {
		return RAM;
	}

	if (address < 0x4020 && address >= 0x2000) {
		return REGISTER;
	}

	if (address < 0x6000 && address >= 0x4020) {
		return EXPANSION_ROM;
	}

	if (address < 0x8000 && address >= 0x6000) {
		return SRAM;
	}

	if (address >= 0x8000) {
		return PRGROM;
	}

	return -1;
}

/* write to RAM */
void write_mem(machine* mch, uint16_t address, uint8_t value)
{
	char location = map_mem(address);
	int i = 0;
	uint16_t mregadr = 0x2000;
	uint16_t curradr = 0x2008;
	uint8_t* memory = mch->memory;

	switch(location) {
		case RAM:
			memory[address] = value;

			if (address == 0x0000) {
				memory[0x0800] = value;
				memory[0x1000] = value;
				memory[0x1800] = value;
			}

			break;

		case REGISTER:
			memory[address] = value;
			// mirror of 0x2000-0x2008 every 8 bytes
			if (address >= 0x2000 && address <= 0x2008) {
				for (i = 0; i < 0x1FF8; i += 8) {
					memory[curradr++] = memory[mregadr++];
					memory[curradr++] = memory[mregadr++];
					memory[curradr++] = memory[mregadr++];
					memory[curradr++] = memory[mregadr++];
					memory[curradr++] = memory[mregadr++];
					memory[curradr++] = memory[mregadr++];
					memory[curradr++] = memory[mregadr++];
					memory[curradr++] = memory[mregadr++];
					memory[curradr++] = memory[mregadr++];
					mregadr = 0x2000;
				}
			}
			break;

		case EXPANSION_ROM:
		case SRAM:
		case PRGROM:
		case CHRROM:
			memory[address] = value;
			break;
	}
}

/* read from appropriate memory location */
uint8_t read_mem(machine* mch, uint16_t address)
{
	char location = map_mem(address);
	uint8_t* memory = mch->memory;
	uint8_t* prgrom = mch->prg_rom;
	uint8_t* chrrom = mch->chr_rom;

	switch(location) {
		case RAM: return memory[address];
		case REGISTER: return memory[address];
		case EXPANSION_ROM: return 0;
		case SRAM: return 0;
		case PRGROM: return prgrom[address];
		case CHRROM: return chrrom[address];
	}

	return -1;
}