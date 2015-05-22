#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define INIT_PC 0 // placeholder
#define INTERRUPT_PERIOD 100 // placeholder
#define CPU_FREQ 

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

void execute_cpu(machine* mch)
{
	switch(mch->memory[mch->pc]){
		case 0xEA:
			fprintf(stdout, "NOP\n");
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
	mch->memory[1] = 0xEA;
	mch->memory[2] = 0xEA;
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