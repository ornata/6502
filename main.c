#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define INIT_PC 0 // placeholder
#define INTERRUPT_PERIOD 100 // placeholder
#define DEBUG 1

#define SET_CARRY(x)   x | 0b00010001
#define CLEAR_CARRY(x) x & 0b11111110
#define SET_ZERO(x)    x | 0b00010010
#define CLEAR_ZERO(x)  x & 0b11111101
#define SET_OVERFLOW(x) x | 0b01010000
#define CLEAR_OVERFLOW(x) x & 0b10111111
#define SET_NEG(x) x | 0b10010000
#define CLEAR_NEG(x) x & 0b01111111

typedef struct machine {
	uint8_t A; // accumulator
	uint8_t X; // x index
	uint8_t Y; // y index
	uint8_t P; // processor status
	uint8_t S; // stack
	uint16_t pc; // program counter
	uint8_t *memory;
} machine;

void execute_cpu(machine* m);
void generate_interrupts();
void emulate_sound();
void emulate_graphics();
void time_sync();

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

/* Print the bits in an uint8_t */
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

/*
* ADD with carry - Add value to dest and update flags.
* Flags affected: S, V, Z, C
*/
void adc(uint8_t value, uint8_t* dest, uint8_t* P)
{
	uint8_t sum = *dest + value;
	uint8_t carry = sum ^ (*dest) ^ value;

	// check for carry
	if (carry & 0b10000000) {
		*P = SET_CARRY(*P); // set bit 0 = carry to 1
	} else {
		*P = CLEAR_CARRY(*P);
	}

	// check for zero
	if (sum == 0b00000000) {
		*P = SET_ZERO(*P);
	} else {
		*P = CLEAR_ZERO(*P);
	}

	// check for overflow
	if ((sum + carry) & 0b10000000) {
		*P = SET_OVERFLOW(*P);
	} else {
		*P = CLEAR_OVERFLOW(*P);
	}

	// check for negative
	if (sum & 0b10000000) {
		SET_NEG(*P);
	} else {
		CLEAR_NEG(*P);
	}
	*dest = sum;
}

/*
* AND - Bitwise AND with accumulator
* Flags affected: S, Z
*/
void and(uint8_t value, uint8_t* A, uint8_t* P)
{
	uint8_t anded = *A & value;

	// check if result is 0
	if (anded == 0b00000000) {
		*P = SET_ZERO(*P);
	} else {
		*P = CLEAR_ZERO(*P);
	}

	// check if result is negative or positive
	if (anded & 0b10000000) {
		SET_NEG(*P);
	} else {
		CLEAR_NEG(*P);
	}

}

void execute_cpu(machine* mch)
{
	uint8_t *opcode = &mch->memory[mch->pc];
	uint8_t *memory = mch->memory;

	switch(*opcode){
		case 0xEA:
			break;

		/* Add: add contents of memory location to accumulator with carry */
		case 0x69: // adc immediate
			adc(opcode[1], &(mch->A), &(mch->P));
			mch->pc += 1;
			break;

		case 0x65: // adc zero page
			fprintf(stdout, "Add: zero page\n");
			break;

		case 0x75:
			fprintf(stdout, "Add: zero page, X\n");
			break;

		case 0x6D: // adc absolute
		{
			adc(memory[opcode[1]], &(mch->A), &(mch->P));
			mch->pc += 1;
			break;
		}
		case 0x7D: // adc absolute,x
		{
			adc(mch->X, &(opcode[1]), &(mch->P)); // add with carry X to opcode
			adc(memory[opcode[1]], &(mch->A), &(mch->P));
			mch->pc += 1;
			break;
		}
		case 0x79: // adc absolute,y
		{
			adc(mch->Y, &(opcode[1]), &(mch->P)); // add with carry Y to opcode
			adc(memory[opcode[1]], &(mch->A), &(mch->P));
			mch->pc += 1;
			break;
		}

		case 0x61: // adc indirect x
		{
			uint8_t top = memory[opcode[1]];
			uint8_t bot = memory[opcode[2]];
			adc(mch->X, &top, &(mch->P));
			uint16_t address = (((uint16_t)top << 8) | bot);
			adc(memory[address], &(mch->A), &(mch->P));
			mch->pc += 2;
			break;
		}
		case 0x71: // adc indirect y
		{
			uint8_t top = memory[opcode[1]];
			uint8_t bot = memory[opcode[2]];
			adc(mch->Y, &top, &(mch->P));
			uint16_t address = (((uint16_t)top << 8) | bot);
			adc(memory[address], &(mch->A), &(mch->P));
			mch->pc += 2;
			break;
		}

		/* AND - and accumulator with memory */
		case 0x29: // and immediate
			fprintf(stdout, "and immediate\n");
			and(opcode[1], &(mch->A), &(mch->P));
			mch->pc += 1;
			break;

		case 0x25: // and zero page
			fprintf(stdout, "and zero page\n");
			break;

		case 0x35: // and zero page, offset by X
			fprintf(stdout, "and zp, x\n");
			break;

		case 0x2D: // and absolute
			fprintf(stdout, "and absolute\n");
			and(memory[opcode[1]], &(mch->A), &(mch->P));
			mch->pc += 1;
			break;

		case 0x3D:
			fprintf(stdout, "and abs, x\n");
			adc(mch->X, &(opcode[1]), &(mch->P));
			and(memory[opcode[1]], &(mch->A), &(mch->P));
			mch->pc += 1;
			break;

		case 0x39:
			fprintf(stdout, "and abs, y\n");
			adc(mch->Y, &(opcode[1]), &(mch->P));
			and(memory[opcode[1]], &(mch->A), &(mch->P));
			mch->pc += 1;
			break;

		case 0x21:
		{
			fprintf(stdout, "and (oper, x)\n");
			uint8_t top = memory[opcode[1]];
			uint8_t bot = memory[opcode[2]];
			adc(mch->X, &top, &(mch->P));
			uint16_t address = (((uint16_t)top << 8) | bot);
			and(memory[address], &(mch->A), &(mch->P));
			mch->pc += 2;
			break;
		}
		case 0x31:
		{
			fprintf(stdout, "and (oper), y\n");
			uint8_t top = memory[opcode[1]];
			uint8_t bot = memory[opcode[2]];
			adc(mch->Y, &top, &(mch->P));
			uint16_t address = (((uint16_t)top << 8) | bot);
			and(memory[address], &(mch->A), &(mch->P));
			mch->pc += 2;
			break;
		}

	}

	mch->pc += 1; // advance program counter
}

int main(int argc, char* argv[])
{
	char running = 1; // avoid compiler treating a constant 1 as a variable, temporarily 0
/*
	FILE* fp;
	fp = fopen(argv[1], "r");

	if (fp == NULL) {
		fprintf(stderr, "Could not open file '%s'. Exiting.\n", argv[1]);
		exit(-1);
	}
	*/

	machine* mch = (machine*) malloc(sizeof(mch));

	if (mch == NULL) {
		fprintf(stderr, "Could not allocate memory. Exiting. \n");
		exit(-2);
	}

	mch->memory = (uint8_t*)malloc(100 * sizeof(uint8_t));

	if (mch->memory == NULL) {
		fprintf(stderr, "Could not allocate memory. Exiting. \n");
		exit(-2);
	}

	mch->P = 0b00001000; // bit 5 is 1 at all times
	mch->memory[0] = 0xEA; // NOP
	mch->memory[1] = 0x69; // ADC
	mch->memory[2] = 0xFF; // value to add to accumulator
	mch->memory[3] = 0x61; // nop
	mch->memory[4] = 0x00; 
	mch->memory[5] = 0x01; 
	mch->memory[6] = 0x61;
	mch->memory[7] = 0;
	mch->memory[8] = 1;
	int len = 8;
	int i;

	for (i = 0; i < len; i++) {
		execute_cpu(mch);
	}

/*
	// main loop. run while cpu is running	
	while(running){
		execute_cpu(mch);
		generate_interrupts();
		emulate_graphics();
		emulate_sound();
		time_sync();
	}
	*/

	free(mch->memory);
	free(mch);
}
