#include <stdint.h>
#include "io.h"
#include "machine.h"

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

/* Read ROM into a machine's memory */
void read_rom (uint8_t* memory, FILE* fp)
{
	char running = 1;
	char len = 0;
	char ch;
	uint8_t mem_entry;
	int i = 0;

	while(running) {

		ch = fgetc(fp);

		if (feof(fp))
			break;

		if (!isalpha(ch) && !isdigit(ch))
			continue;

		if (isalpha(ch) && toupper(ch) > 'F')
			continue;

		if (isalpha(ch)) {
			ch = toupper(ch);
			ch -= 55;
		} else {
			ch -= 48;
		}

		if (len == 0) {
			mem_entry |= (ch << 4);
			++len;
		} else {
			mem_entry |= ch;
			memory[i++] = mem_entry;
			mem_entry = 0;
			--len;
		}

	}
}