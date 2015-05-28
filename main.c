#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "machine.h"
#include "opcodes.h"
#include "io.h"

#define INIT_PC 0 // placeholder
#define INTERRUPT_PERIOD 100 // placeholder
#define DEBUG 1

void execute_cpu(machine* m);

// run appropriate function for opcode in memory
void execute_cpu(machine* mch)
{
	uint8_t *opcode = &mch->memory[mch->pc];
	uint8_t *memory = mch->memory;

	switch(*opcode){
		/* NOP */
		case 0xEA: return nop(mch);

		/* Add: add contents of memory location to accumulator with carry */
		case 0x69: return adc_imm(opcode[1], mch);
		case 0x65: return adc_zp(opcode[1], mch);
		case 0x75: return adc_zpx(opcode[1], mch);
		case 0x6D: return adc_abs(opcode[1], opcode[2], mch);
		case 0x7D: return adc_absx(opcode[1], opcode[2], mch);
		case 0x79: return adc_absy(opcode[1], opcode[2], mch);
		case 0x61: return adc_indx(opcode[1], opcode[2], mch);
		case 0x71: return adc_indy(opcode[1], opcode[2], mch);

		/* AND - and accumulator with memory */
		case 0x29: return and_imm(opcode[1], mch);
		case 0x25: return and_zp(opcode[1], mch);
		case 0x35: return and_zpx(opcode[1], mch);
		case 0x2D: return and_abs(opcode[1], opcode[2], mch);
		case 0x3D: return and_absx(opcode[1], opcode[2], mch);
		case 0x39: return and_absy(opcode[1], opcode[2], mch);
		case 0x21: return and_indx(opcode[1], opcode[2], mch);
		case 0x31: return and_indy(opcode[1], opcode[2], mch);

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
		case 0x18: return clc(mch);

		/* CLI - clear interrupt disable bit */
		case 0x58: return cli(mch);

		/* CLV - clear overflow */
		case 0xB8: return clv(mch);

		/* CMP - compare memory and accumulator */
		case 0xC9: return cmp_imm(opcode[1], mch);
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

		/* CPX - compare memory and index X */
		case 0xE0: return cpx_imm(opcode[1], mch);
		case 0xE4: // zero page
			mch->cycle += 3;
			exit(1);
			break;
		case 0xEC: // absolute
			mch->cycle += 4;
			exit(1);
			break;

		/* CPY - compare memory and index Y */
		case 0xC0: return cpy_imm(opcode[1], mch);
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
		case 0xCA: return dex(mch);

		/* DEY - decrement Y by 1 */
		case 0x88: return dey(mch);

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
		case 0x4C: jmp(opcode[1], opcode[2], mch);
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
		mch->pc += 1; // advance program counter
		printf("cpu cycle: %d\n", mch->cycle);
	}

	free(mch->memory);
	free(mch);
	fclose(fp);
}
