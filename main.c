#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "machine.h"
#include "opcodes.h"
#include "io.h"
#include "graphics.h"
#include "sound.h"

#define INIT_PC 0 // placeholder
#define INTERRUPT_PERIOD 100 // placeholder
#define DEBUG 1

void execute_cpu(machine* m);

// run appropriate function for opcode in memory
void execute_cpu(machine* mch)
{
	uint8_t *opcode = &mch->memory[mch->pc++];
	uint8_t *memory = mch->memory;

	fprintf(stdout, "opcode: %x\n", opcode[0]);

	switch(*opcode) {
		case 0x02: exit(123);
		case 0x12: exit(123);
		case 0x22: exit(123);
		case 0x32: exit(123);
		case 0x42: exit(123);
		case 0x52: exit(123);
		case 0x62: exit(123);
		case 0x72: exit(123);
		case 0x92: exit(123);
		case 0xB2: exit(123);
		case 0xD2: exit(123);
		case 0xF2: exit(123);
		case 0xEA: return nop(mch, 1);
		case 0x1A: return nop(mch, 2); // illegal instruction (nop with 2 cycles)
		case 0x7A: return nop(mch, 2); // same as above
		case 0x69: return adc_imm(opcode[1], mch);
		case 0x65: return adc_zp(opcode[1], mch);
		case 0x75: return adc_zpx(opcode[1], mch);
		case 0x6D: return adc_abs(opcode[2], opcode[1], mch);
		case 0x7D: return adc_absx(opcode[2], opcode[1], mch);
		case 0x79: return adc_absy(opcode[2], opcode[1], mch);
		case 0x61: return adc_indx(opcode[2], opcode[1], mch);
		case 0x71: return adc_indy(opcode[2], opcode[1], mch);
		case 0x29: return and_imm(opcode[1], mch);
		case 0x25: return and_zp(opcode[1], mch);
		case 0x35: return and_zpx(opcode[1], mch);
		case 0x2D: return and_abs(opcode[2], opcode[1], mch);
		case 0x3D: return and_absx(opcode[2], opcode[1], mch);
		case 0x39: return and_absy(opcode[2], opcode[1], mch);
		case 0x21: return and_indx(opcode[2], opcode[1], mch);
		case 0x31: return and_indy(opcode[2], opcode[1], mch);
		case 0x0A: return asl_acc(mch);
		case 0x06: return asl_zp(opcode[1], mch);
		case 0x16: return asl_zpx(opcode[1], mch);
		case 0x0E: return asl_abs(opcode[2], opcode[1], mch);
		case 0x1E: return asl_absx(opcode[2], opcode[1], mch);
		case 0x90: return branch_clear(opcode[2], opcode[1], mch, 0b00000001);
		case 0xB0: return branch_set(opcode[2], opcode[1], mch, 0b00000001);
		case 0xF0: return branch_set(opcode[2], opcode[1], mch, 0b00000010);
		case 0x24: return bit_zp(opcode[1], mch);
		case 0x2C: return bit_abs(opcode[2], opcode[1], mch);
		case 0x30: return branch_set(opcode[2], opcode[1], mch, 0b10000000);
		case 0xD0: return branch_clear(opcode[2], opcode[1], mch, 0b00000010);
		case 0x10: return branch_clear(opcode[2], opcode[1], mch, 0b10000000);
		case 0x00: return brk(mch);
		case 0x50: return branch_clear(opcode[2], opcode[1], mch, 0b01000000);
		case 0x70: return branch_set(opcode[2], opcode[1], mch, 0b01000000);
		case 0x18: return clc(mch);
		case 0x58: return cli(mch);
		case 0xB8: return clv(mch);
		case 0xC9: return cmp_imm(opcode[1], mch);
		case 0xC5: return cmp_zp(opcode[1], mch);
		case 0xD5: return cmp_zpx(opcode[1], mch);
		case 0xCD: return cmp_abs(opcode[2], opcode[1], mch, 0, 0);
		case 0xDD: return cmp_abs(opcode[2], opcode[1], mch, 1, mch->X);
		case 0xD9: return cmp_abs(opcode[2], opcode[1], mch, 1, mch->Y);
		case 0xC1: return cmp_indy(opcode[2], opcode[1], mch);
		case 0xD1: return cmp_indx(opcode[2], opcode[1], mch);
		case 0xE0: return cpx_imm(opcode[1], mch);
		case 0xE4: // zero page
			mch->cycle += 3;
			exit(1);
			break;
		case 0xEC: // absolute
			mch->cycle += 4;
			exit(1);
			break;

		case 0xC0: return cpy_imm(opcode[1], mch);
		case 0xC4: // zero page
			mch->cycle += 3;
			exit(1);
			break;
		case 0xCC: // absolute
			mch->cycle += 4;
			exit(1);
			break;

		case 0xC6: return dec_zp(opcode[1], mch);
		case 0xD6: return dec_zpx(opcode[1], mch);
		case 0xCE: return dec_abs(opcode[2], opcode[1], mch);
		case 0xDE: return dec_absx(opcode[2], opcode[1], mch);
		case 0xCA: return dex(mch);
		case 0x88: return dey(mch);
		case 0x49: return eor_imm(opcode[1], mch);
		case 0x45: return eor_zp(opcode[1], mch);
		case 0x55: return eor_zpx(opcode[1], mch);
		case 0x40: return eor_abs(opcode[2], opcode[1], mch);
		case 0x5D: return eor_absx(opcode[2], opcode[1], mch);
		case 0x59: return eor_absy(opcode[2], opcode[1], mch);
		case 0x41: return eor_indx(opcode[2], opcode[1], mch);
		case 0x51: return eor_indy(opcode[2], opcode[1], mch);

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

		case 0x4C: jmp_abs(opcode[2], opcode[1], mch);
		case 0x6C: jmp_ind(opcode[2], opcode[1], mch);

		/* JSR - jump and save return address */
		case 0x20:
			mch->cycle += 6;
			exit(1);
			break;

		case 0xA9: return lda_imm(opcode[1], mch);
		case 0xA5: return lda_zp(opcode[1], mch, 0, 0);
		case 0xB5: return lda_zp(opcode[1], mch, 1, mch->X);
		case 0xAD: return lda_abs(opcode[2], opcode[1], mch, 0, 0);
		case 0xBD: return lda_abs(opcode[2], opcode[1], mch, 1, mch->X);
		case 0xB9: return lda_abs(opcode[2], opcode[1], mch, 1, mch->Y);
		case 0xA1: return lda_indx(opcode[2], opcode[1], mch);
		case 0xB1: return lda_indy(opcode[2], opcode[1], mch);
		case 0xA2: return ldx_imm(opcode[1], mch);
		case 0xA6: return ldx_zp(opcode[1], mch, 0, 0);
		case 0xB6: return ldx_zp(opcode[1], mch, 1, mch->X);
		case 0xAE: return ldx_abs(opcode[2], opcode[1], mch, 0, 0);
		case 0xBE: return ldx_abs(opcode[2], opcode[1], mch, 1, mch->X);
		case 0xA0: return ldy_imm(opcode[1], mch);
		case 0xA4: return ldy_zp(opcode[1], mch, 0, 0);
		case 0xB4: return ldy_zp(opcode[1], mch, 1, mch->X);
		case 0xAC: return ldy_abs(opcode[2], opcode[1], mch, 0, 0);
		case 0xBC: return ldy_abs(opcode[2], opcode[1], mch, 1, mch->X);
		case 0x4A: return lsr_acc(mch);
		case 0x46: return lsr_zp(opcode[1], mch);
		case 0x56: return lsr_zpx(opcode[1], mch);
		case 0x4E: return lsr_abs(opcode[2], opcode[1], mch);
		case 0x5E: return lsr_absx(opcode[2], opcode[1], mch);
		case 0x09: return or_imm(opcode[1], mch);
		case 0x05: return or_zp(opcode[1], mch);
		case 0x15: return or_zpx(opcode[1], mch);
		case 0x0D: return or_abs(opcode[2], opcode[1], mch);
		case 0x1D: return or_absx(opcode[2], opcode[1], mch);
		case 0x19: return or_absy(opcode[2], opcode[1], mch);
		case 0x01: return or_indx(opcode[2], opcode[1], mch);
		case 0x11: return or_indy(opcode[2], opcode[1], mch);
		case 0x48: return pha(mch);
		case 0x08: return php(mch);
		case 0x68: return pla(mch);
		case 0x28: return plp(mch);
		case 0x60: return rts(mch);
		case 0x78: return sei(mch); 
		case 0x38: return sec(mch);
		case 0xAA: return tax(mch);
		case 0xA8: return tay(mch);
		case 0x8A: return txa(mch);
		case 0x98: return tya(mch);

		default:
			fprintf(stdout, "unimplemented opcode!\n");
			fprintf(stdout, "opcode in question: %x\n", opcode[0]);
			exit(1);
			break;
	}
}

int main(int argc, char* argv[])
{
	char running = 1; // avoid compiler treating a constant 1 as a variable, temporarily 0

	FILE* fp;
	fp = fopen(argv[1], "rb");

	if (fp == NULL) {
		fprintf(stderr, "Could not open file '%s'. Exiting.\n", argv[1]);
		exit(-1);
	}

	machine* mch = (machine*) malloc(sizeof(mch));
	
	if (mch == NULL) {
		fprintf(stderr, "Could not allocate memory. Exiting. \n");
		exit(-2);
	}

	mch->P = 0b00100000; // bit 5 is 1 at all times
	mch->cycle = 0;

	mch->stack = (uint8_t*) malloc(256 * sizeof(uint8_t));

	if (mch->stack == NULL) {
		fprintf(stderr, "Could not allocate memory. Exiting. \n");
		exit(-2);
	}

	mch->stack_head = 0;

	read_file(mch, fp);

	while(running){
		print_machine_state(mch);
		execute_cpu(mch);
		mch->pc += 1;
		printf("cpu cycle: %d\n", mch->cycle);
		// check for interrupts
		/*if ((mch->P & 0b00000100) != 0) {
			printf("Caught an interrupt\n");
			mch->stack_head++;
			mch->stack[mch->stack_head] = mch->pc;
			mch->pc = ((uint16_t)mch->memory[0xFFFE] << 8) | mch->memory[0xFFFF];
		}*/
		//emulate_graphics(mch->memory);
		//emulate_sound(mch->memory);
	}

	free(mch->memory);
	free(mch->prg_rom);
	free(mch->prg_ram);
	free(mch->chr_rom);
	free(mch);
	fclose(fp);
}
