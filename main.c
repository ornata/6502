#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "opcodes.h"
#include "debug_io.h"

#define INIT_PC 0 // placeholder
#define INTERRUPT_PERIOD 100 // placeholder
#define DEBUG 1

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

void execute_cpu(machine* m);

// run appropriate function for opcode in memory
void execute_cpu(machine* mch)
{
	uint8_t *opcode = &mch->memory[mch->pc];
	uint8_t *memory = mch->memory;

	switch(*opcode){
		/* NOP */
		case 0xEA:
			mch->cycle += 1;
			break;

		/* Add: add contents of memory location to accumulator with carry */
		case 0x69: // adc immediate
			adc(opcode[1], &(mch->A), &(mch->P));
			mch->pc += 1;
			mch->cycle += 2;
			break;

		case 0x65: // adc zero page
			fprintf(stdout, "Add: zero page\n");
			mch->pc += 1;
			mch->cycle += 2;
			break;

		case 0x75: // adc zero page, offset by X
			fprintf(stdout, "Add: zero page, X\n");
			mch->cycle += 2;
			break;

		case 0x6D: // adc absolute
		{
			adc(memory[opcode[1]], &(mch->A), &(mch->P));
			mch->pc += 1;
			mch->cycle += 3;
			break;
		}

		case 0x7D: // adc absolute,x
		{
			adc(mch->X, &(opcode[1]), &(mch->P)); // add with carry X to opcode
			adc(memory[opcode[1]], &(mch->A), &(mch->P));
			mch->pc += 1;
			mch->cycle += 3;
			break;
		}

		case 0x79: // adc absolute,y
		{
			adc(mch->Y, &(opcode[1]), &(mch->P)); // add with carry Y to opcode
			adc(memory[opcode[1]], &(mch->A), &(mch->P));
			mch->pc += 1;
			mch->cycle += 3;
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
			mch->cycle += 2;
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
			mch->cycle += 2;
			break;
		}

		/* AND - and accumulator with memory */
		case 0x29: // and immediate
			fprintf(stdout, "and immediate\n");
			and(opcode[1], &(mch->A), &(mch->P));
			mch->pc += 1;
			mch->cycle += 2;
			break;

		case 0x25: // and zero page
			fprintf(stdout, "and zero page\n");
			mch->cycle += 2;
			break;

		case 0x35: // and zero page, offset by X
			fprintf(stdout, "and zp, x\n");
			mch->cycle += 2;
			break;

		case 0x2D: // and absolute
			fprintf(stdout, "and absolute\n");
			and(memory[opcode[1]], &(mch->A), &(mch->P));
			mch->pc += 1;
			mch->cycle += 3;
			break;

		case 0x3D: // and absolute, offset by X
			fprintf(stdout, "and abs, x\n");
			adc(mch->X, &(opcode[1]), &(mch->P));
			and(memory[opcode[1]], &(mch->A), &(mch->P));
			mch->pc += 1;
			mch->cycle += 3;
			break;

		case 0x39: // and absolute, offset by Y
			fprintf(stdout, "and abs, y\n");
			adc(mch->Y, &(opcode[1]), &(mch->P));
			and(memory[opcode[1]], &(mch->A), &(mch->P));
			mch->pc += 1;
			mch->cycle += 3;
			break;

		case 0x21: // and indirect, offset by X
		{
			fprintf(stdout, "and (oper, x)\n");
			uint8_t top = memory[opcode[1]];
			uint8_t bot = memory[opcode[2]];
			adc(mch->X, &top, &(mch->P));
			uint16_t address = (((uint16_t)top << 8) | bot);
			and(memory[address], &(mch->A), &(mch->P));
			mch->pc += 2;
			mch->cycle += 2;
			break;
		}

		case 0x31: // and indirect, offset by Y
		{
			fprintf(stdout, "and (oper), y\n");
			uint8_t top = memory[opcode[1]];
			uint8_t bot = memory[opcode[2]];
			adc(mch->Y, &top, &(mch->P));
			uint16_t address = (((uint16_t)top << 8) | bot);
			and(memory[address], &(mch->A), &(mch->P));
			mch->pc += 2;
			mch->cycle += 2;
			break;
		}

		/* ASL - arithmetic shift left for memory or accumulator */
		case 0x0A: // accumulator
			mch->cycle += 2;
			exit(1);
			break;
		case 0x06: // zero page
			mch->cycle += 5;
			exit(1);
			break;
		case 0x16: // zero page, offset by X
			mch->cycle += 6;
			exit(1);
			break;
		case 0x0E: // absolute
			mch->cycle += 6;
			exit(1);
			break;
		case 0x1E: // absolute, offset by X
			mch->cycle += 7;
			exit(1);
			break;

		/* BCC - Branch on carry clear */
		case 0x90:
			mch->cycle += 2; // this should differ based off if branch occurs in the same page
			exit(1);
			break;

		/* BCS - Branch on carry set */
		case 0xB0:
			mch->cycle += 2; // same as bcc -- page matters
			exit(1);
			break;

		/* BEQ - Branch on 0 set */
		case 0xF0:
			mch->cycle += 2; // general branching page-ness needs to go here
			exit(1);
			break;

		/* BIT - tests bits in memory with accumulator */
		case 0x24: // zero page
			mch->cycle += 3;
			exit(1);
			break;
		case 0x2C: //absolute
			mch->cycle += 4;
			exit(1);
			break;

		/* BMI - branch on negative set */
		case 0x30:
			mch->cycle += 2; // branch paginess goes here
			exit(1);
			break;

		/* BNE - branch on 0 clear */
		case 0xD0:
			mch->cycle += 2;
			exit(1);
			break;

		/* BPL - branch on negative clear */
		case 0x10:
			mch->cycle += 2;
			exit(1);
			break;

		/* BRK - force break. cannot be masked by setting I!!*/
		case 0x00:
			mch->cycle += 7;
			exit(1);
			break;

		/* BVC - branch on overflow clear */
		case 0x50:
			mch->cycle += 2; //remember paginess
			exit(1);
			break;

		/* BVS - branch on overflow set */
		case 0x70:
			mch->cycle += 2; // paginess
			exit(1);
			break;

		/* CLC - clear carry */
		case 0x18:
			mch->cycle += 2;
			exit(1);
			break;

		/* CLI - clear interrupt disable bit */
		case 0x58:
			mch->cycle += 2;
			exit(1);
			break;

		/* CLV - clear overflow */
		case 0xB8:
			mch->cycle += 2;
			exit(1);
			break;

		/* CMP - compare memory and accumulator */
		case 0xC9: // immediate
			mch->cycle += 2;
			exit(1);
			break;
		case 0xC5: // zero page
			mch->cycle += 3;
			exit(1);
			break;
		case 0xD5: // zero page, offset by X
			mch->cycle += 4;
			exit(1);
			break;
		case 0xCD: // absolute
			mch->cycle += 4;
			exit(1);
			break;
		case 0xDD: // absolute, offset by X
			mch->cycle += 4; // add 1 if page boundary crossed
			exit(1);
			break;
		case 0xD9: // absolute, offset by Y
			mch->cycle += 4; // add 1 if pg boundary crossed
			exit(1);
			break;
		case 0xC1: // (indirect, X)
			mch->cycle += 6;
			exit(1);
			break;
		case 0xD1:
			mch->cycle += 5; // add 1 if pg boundary crossed
			exit(1);
			break;

		/* CPX - compate memory and index X */
		case 0xE0: // immediate
			mch->cycle += 2;
			exit(1);
			break;
		case 0xE4: // zero page
			mch->cycle += 3;
			exit(1);
			break;
		case 0xEC: // absolute
			mch->cycle += 4;
			exit(1);
			break;

		/* CPY - compate memory and index Y */
		case 0xC0: // immediate
			mch->cycle += 2;
			exit(1);
			break;
		case 0xC4: // zero page
			mch->cycle += 3;
			exit(1);
			break;
		case 0xCC: // absolute
			mch->cycle += 4;
			exit(1);
			break;


		/* DEC - decrement memory by 1 */
		case 0xC6: // zero page
			mch->cycle += 5;
			exit(1);
			break;
		case 0xD6: // zero page, offset by X
			mch->cycle += 6;
			exit(1);
			break;
		case 0xCE: // absolute
			mch->cycle += 6;
			exit(1);
			break;
		case 0xDE: // absolute, offset by X
			mch->cycle += 7;
			exit(1);
			break;

		/* DEX - decrement X by 1 */
		case 0xCA:
			mch->cycle += 2;
			exit(1);
			break;

		/* DEY - decrement Y by 1 */
		case 0x88:
			mch->cycle += 2;
			exit(1);
			break;

		/* EOR - xor with accumulator */
		case 0x49: // immediate
			mch->cycle += 2;
			exit(1);
			break;
		case 0x45: // zero page
			mch->cycle += 3;
			exit(1);
			break;
		case 0x55: // zero page, offset by X
			mch->cycle += 4;
			exit(1);
			break;
		case 0x40: // absolute
			mch->cycle += 4;
			exit(1);
			break;
		case 0x5D: // absolute, offset by X
			mch->cycle += 4; // +1 if pg boundary crossed
			exit(1);
			break;
		case 0x59: // absolute, offset by Y
			mch->cycle += 4; // +1 if pg boundary crossed
			exit(1);
			break;
		case 0x41: // (indirect, X)
			mch->cycle += 6;
			exit(1);
			break;
		case 0x51: // (indirect), Y
			mch->cycle += 5; // +1 if pg boundary crossed
			exit(1);
			break;

		/* INC - increment memory by 1 */
		case 0xE6: // zero page
			mch->cycle += 5;
			exit(1);
			break;
		case 0xF6: // zero page, X
			mch->cycle += 6;
			exit(1);
			break;
		case 0xEE: // absolute
			mch->cycle += 6;
			exit(1);
			break;
		case 0xFE: // absolute, X
			mch->cycle += 7;
			exit(1);
			break;

		/* INX - increment X index by 1 */
		case 0xE8:
			mch->cycle += 2;
			exit(1);
			break;

		/* INY - increment Y index by 1 */
		case 0xC8:
			mch->cycle += 2;
			exit(1);
			break;

		/* JMP - jump to new location */
		case 0x4C: // absolute
			mch->cycle += 3;
			exit(1);
			break;
		case 0x6C:
			mch->cycle += 5;
			exit(1);
			break;

		/* JSR - jump and save return address */
		case 0x20:
			mch->cycle += 6;
			exit(1);
			break;

		/* LDA - load memory into accumulator */
		case 0xA9: // immediate
			mch->cycle += 2;
			exit(1);
			break;
		case 0xA5: // zero page
			mch->cycle += 3;
			exit(1);
			break;
		case 0xB5: // zero page, offset by X
			mch->cycle += 4;
			exit(1);
			break;
		case 0xAD: // absolute
			mch->cycle += 4;
			exit(1);
			break;
		case 0xBD: // absolute, offset by X
			mch->cycle += 4; // +1 if pg boundary crossed
			exit(1);
			break;
		case 0xB9:
			mch->cycle += 4; // +1 if pg boundary crossed
			exit(1);
			break;
		case 0xA1: // (indirect, X)
			mch->cycle += 6;
			exit(1);
			break;
		case 0xB1: // (indirect, X)
			mch->cycle += 5; // +1 if pg boundary crossed
			exit(1);
			break;

		/* LDX - load into X */
		case 0xA2: // immediate
			mch->cycle += 2;
			exit(1);
			break;
		case 0xA6: // zero page
			mch->cycle += 3;
			exit(1);
			break;
		case 0xB6: // zero page, Y
			mch->cycle += 4;
			exit(1);
			break;
		case 0xAE: // absolute
			mch->cycle += 4;
			exit(1);
			break;
		case 0xBE: // absolute, Y
			mch->cycle += 4; // +1 if pg boundary crossed
			exit(1);
			break;

		/* LDY - load into Y */
		case 0xA0: // immediate
			mch->cycle += 2;
			exit(1);
			break;
		case 0xA4: // zero page
			mch->cycle += 3;
			exit(1);
			break;
		case 0xB4: // zero page, X
			mch->cycle += 4;
			exit(1);
			break;
		case 0xAC: // absolute
			mch->cycle += 4;
			exit(1);
			break;
		case 0xBC: // absolute, X
			mch->cycle += 4; // +1 if pg boundary crossed
			exit(1);
			break;


		/* LSR - shift mem or accumulator right one bit */
		case 0x4A: // accumulator
			mch->cycle += 2;
			exit(1);
			break;
		case 0x46: // zero page
			mch->cycle += 5;
			exit(1);
			break;
		case 0x56: // zero page, X
			mch->cycle += 6;
			exit(1);
			break;
		case 0x4E: // absolute
			mch->cycle += 6;
			exit(1);
			break;
		case 0x5E: // absolute, X
			mch->cycle += 7;
			exit(1);
			break;

		/* ORA - or memory with accumulator */
		case 0x09: // immediate
			mch->cycle += 2;
			exit(1);
			break;
		case 0x05: // zero page
			mch->cycle += 3;
			exit(1);
			break;
		case 0x15: // zero page, X
			mch->cycle += 4;
			exit(1);
			break;
		case 0x0D: // absolute
			mch->cycle += 4;
			exit(1);
			break;
		case 0x1D: // absolute, X
			mch->cycle += 4; // +1 on pg crossing
			exit(1);
			break;
		case 0x19: // absolute, Y
			mch->cycle += 4; // +1 on pg crossing
			exit(1);
		case 0x01: // (indirect, X)
			mch->cycle += 6;
			exit(1);
			break;
		case 0x11: // (indirect), Y
			mch->cycle += 5;
			exit(1);
			break;

		default:
			fprintf(stdout, "unimplemented opcode!\n");
			exit(1);
			break;

	mch->pc += 1; // advance program counter
	}
}

// read ROM into machine's memory
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


int main(int argc, char* argv[])
{
	char running = 1; // avoid compiler treating a constant 1 as a variable, temporarily 0

	FILE* fp;
	fp = fopen(argv[1], "r");

	if (fp == NULL) {
		fprintf(stderr, "Could not open file '%s'. Exiting.\n", argv[1]);
		exit(-1);
	}

	machine* mch = (machine*) malloc(sizeof(mch));

	if (mch == NULL) {
		fprintf(stderr, "Could not allocate memory. Exiting. \n");
		exit(-2);
	}

	mch->memory = (uint8_t*)malloc(65536 * sizeof(uint8_t)); // 2 kB internal memory
	if (mch->memory == NULL) {
		fprintf(stderr, "Could not allocate memory. Exiting. \n");
		exit(-2);
	}

	mch->P = 0b00010000; // bit 5 is 1 at all times
	mch->cycle = 0;
	read_rom(mch->memory, fp);

	// main loop. run while cpu is running	
	while(running){
		execute_cpu(mch);
		printf("cpu cycle: %d\n", mch->cycle);
	}

	free(mch->memory);
	free(mch);
	fclose(fp);
}
