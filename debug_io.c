#include <stdio.h>
#include <stdint.h>

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