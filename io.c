#include <stdint.h>
#include "io.h"
#include "machine.h"
#define PRINT_IO 0

/* Print the bits in an uint8_t */
void print_bits(uint8_t x)
{
	uint32_t b = 0x80;
	while (b != 0){
		fprintf(stdout, "%d", (b & x) ? 1 : 0);
		x ^= b; // turn off b-th bit
		b >>= 1;
	}
	fprintf(stdout,"\n");
} 

/* Print the bits in an uint16_t */
void print_bits16(uint16_t x)
{
	uint32_t b = 0x8000;
	while (b != 0){
		fprintf(stdout, "%d", (b & x) ? 1 : 0);
		x ^= b; // turn off b-th bit
		b >>= 1;
	}
	fprintf(stdout,"\n");
}


void read_ines(machine* mch, FILE* fp)
{
	int prg_rom_size, chr_rom_size;
	uint8_t flags6, flags7, flags9, flags10;
	int prg_ram_size;

	// read header
	fseek(fp, 4, SEEK_SET); // skip past NES and MS-DOS newline
	prg_rom_size = fgetc(fp) * 16384;
	chr_rom_size = fgetc(fp) * 8192;
	flags6 = fgetc(fp);
	flags7 = fgetc(fp);
	prg_ram_size = fgetc(fp);
	prg_ram_size = 0 ? 8192 : 8192 * prg_ram_size;
	flags9 = fgetc(fp);
	flags10 = fgetc(fp); // unofficial
	fseek(fp, 5, SEEK_CUR);

	// allocate memory for rom/ram
	mch->prg_rom = (uint8_t*) malloc(prg_rom_size * sizeof(uint8_t));
	if (!mch->prg_rom) {
		fprintf(stderr, "Could not allocate memory!\n");
		exit(-1);
	}

	mch->chr_rom = (uint8_t*) malloc(chr_rom_size * sizeof(uint8_t));
	if (!mch->chr_rom) {
		fprintf(stderr, "Could not allocate memory!\n");
		exit(-1);
	}

	mch->memory = (uint8_t*) malloc(2048 * sizeof(uint8_t));
	if (!mch->memory) {
		fprintf(stderr, "Could not allocate memory!\n");
		exit(-1);
	}

	mch->prg_ram = (uint8_t*) malloc(prg_ram_size * sizeof(uint8_t));
	if (!mch->prg_ram) {
		fprintf(stderr, "Could not allocate memory!\n");
		exit(-1);
	}

	// read in the prg rom data
	int i, j;
	for (i = 0; i < prg_rom_size; i++){
		mch->prg_rom[i] = fgetc(fp);
	}
	// read in the chr rom data
	for (i = 0; i < chr_rom_size; i++){
		mch->chr_rom[i] = fgetc(fp);
	}
	// todo: read in playchoice inst-rom if present
	// todo: read in playchoide prom if present
	// todo: read the 127 or 128 byte title if present
}
