#ifndef MACHINE_H
#define MACHINE_H

// structure that contains all info about the machine at current time
typedef struct machine {
	uint8_t A; // accumulator
	uint8_t X; // x index
	uint8_t Y; // y index
	uint8_t P; // processor status
	uint8_t S; // stack
	uint16_t pc; // program counter
	uint8_t *memory;
	int cycle;
} machine;

#endif