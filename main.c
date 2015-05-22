#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define INIT_PC 0 // placeholder
#define INTERRUPT_PERIOD 100 // placeholder

#define SET_CARRY(x)   x | 0b00000001
#define CLEAR_CARRY(x) x & 0b11111110
#define SET_ZERO(x)    x | 0b00000010
#define CLEAR_ZERO(x)  x & 0b11111101
#define SET_OVERFLOW(x) x | 0b01000000
#define CLEAR_OVERFLOW(x) x & 0b10111111
#define SET_NEG(x) x | 0b10000000
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
	uint32_t b = 0b10000000;
	while (b != 0){
		fprintf(stdout, "%d", (b & x) ? 1 : 0);
		x ^= b; // turn off b-th bit
		b >>= 1;
	}
	fprintf(stdout,"\n");
}

void execute_cpu(machine* mch)
{

	// current opcode
	uint8_t *opcode = &mch->memory[mch->pc];

	switch(*opcode){
		case 0xEA:
			fprintf(stdout, "NOP\n");
			break;

		/* Add: add contents of memory location to accumulator with carry */
		case 0x69:
			fprintf(stdout, "ADC #<value>\n");
			fprintf(stdout, "<value> = %d\n", opcode[1]);
			fprintf(stdout, "A = %d\n", mch->A);

			uint8_t sum = mch->A + opcode[1];
			uint8_t carry = (sum) ^ mch->A ^ opcode[1];

			// check for carry
			if (carry & 0b10000000) {
				mch->P = SET_CARRY(mch->P); // set bit 0 = carry to 1
			} else {
				mch->P = CLEAR_CARRY(mch->P);
			}

			// check for zero
			if (sum == 0b00000000) {
				mch->P = SET_ZERO(mch->P);
			} else {
				mch->P = CLEAR_ZERO(mch->P);
			}

			// check for overflow
			if ((sum + carry) & 0b10000000) {
				mch->P = SET_OVERFLOW(mch->P);
			} else {
				mch->P = CLEAR_OVERFLOW(mch->P);
			}

			// check for negative
			if (mch->P & 0b10000000) {
				SET_NEG(mch->P);
			} else {
				CLEAR_NEG(mch->P);
			}

			mch->A = sum;
			fprintf(stdout, "A + value = %d\n", mch->A);
			fprintf(stdout, "P: ");
			print_bits(mch->P);
			mch->pc += 1;
			break;

		case 0x65:
			fprintf(stdout, "Add: zero page\n");
			break;
		case 0x75:
			fprintf(stdout, "Add: zero page, X\n");
			break;
		case 0x6D:
			fprintf(stdout, "Add: absolute\n");
			break;
		case 0x7D:
			fprintf(stdout, "Add: absolute, X\n");
			break;
		case 0x79:
			fprintf(stdout, "Add: absolute, Y\n");
			break;
		case 0x61:
			fprintf(stdout, "Add: (indirect, X)\n");
			break;
		case 0x71:
			fprintf(stdout, "Add: (indirect), Y\n");
			break;
	}		

	mch->pc += 1; // advance program counter
}

int main(int argc, char* argv[])
{
	int32_t cycles_until_interrupt = INTERRUPT_PERIOD; // how many cycles until next we expect the next interrupt?
	char running = 1; // avoid compiler treating a constant 1 as a variable, temporarily 0
	char cycles[140]; // how many cycles does the current opcode take?

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
	mch->memory[0] = 0xEA; // NOP
	mch->memory[1] = 0x69; // ADC
	mch->memory[2] = 0xFF; // value to add to accumulator
	mch->memory[3] = 0xEA; // nop
	execute_cpu(mch);
	execute_cpu(mch);
	execute_cpu(mch);
	execute_cpu(mch);

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