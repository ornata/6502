#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define INIT_PC 0 // placeholder
#define INTERRUPT_PERIOD 100 // placeholder

int main(void)
{
	uint32_t cycles_until_interrupt = INTERRUPT_PERIOD; // how many cycles until next we expect the next interrupt?
	uint16_t pc = INIT_PC; // program counter
	char running = 1; // avoid compiler treating a constant 1 as a variable
	uint16_t opcode;
	uint16_t mem[100]; // placeholder

	// main loop. run while cpu is running	
	while(running){

		// fetch opcode
		opcode = mem[pc]; // get next opcode from memory
		++pc;
		cycles_until_interrupt -= cycles[opcode]; // update number of cycles until we expect next interrupt

		// handle and interpret opcode
		switch(opcode)
		{
			/* loads and stores */

			// lda - load accumulator
			case (0xA9):
				printf("LDA #Oper\n");
				break;
			case (0xA5):
				printf("LDA Zpg\n");
				break;
			case (0xB5):
				printf("LDA Zpg, X\n");
				break;
			case (0xAD):
				printf("LDA Abs\n");
				break;
			case (0xBD):
				printf("LDA Abs, X\n");
				break;
			case (0xB9):
				printf("LDA Abs, Y\n");
				break;
			case (0xA1):
				printf("LDA (Zpg, X)\n");
				break;
			case (0xB1):
				printf("LDA (Zpg),Y\n");
				break;
			case (0xB2):
				printf("LDA (Zpg)\n");
				break;
/*
			case (ldx):
				printf("ldx - load x index\n");
				break;
			case (ldy):
				printf("ldy - load y index\n");
				break;
			case (sta):
				printf("sta - store accumulator\n");
				break;
			case (stx):
				printf("sty - store x index\n");
				break;
			case (sty):
				printf("sty - store y index\n");
				break;
			case (stz):
				printf("stz - store zero\n");
				break;

			// stack ops
			case (pha):
				printf("pha - push accumulator\n");
				break;
			case (phx):
				printf("phx - push x index\n");
				break;
			case (phy):
				printf("phy - push y index\n");
				break;
			case (php):
				printf("php - push processor flags\n");
				break;
			case (pla):
				printf("pla - pop accumulator\n");
				break;
			case (plx):
				printf("plx - pop x index\n");
				break;
			case (ply):
				printf("ply - pop y index\n");
				break;
			case (plp):
				printf("plp - pop processor flags\n");
				break;
			case (tsx):
				printf("tsx - transfer stack pointer to x\n");
				break;
			case (txs):
				printf("txs - transfer stack pointer to x\n");
				break;

			// increments and decrements
			case (ina):
				printf("ina - increment accumulator\n");
				break;
			case (inx):
				printf("inx - increment x index\n");
				break;
			case (iny):
				printf("iny - increment y index\n");
				break;
			case (dea):
				printf("dea - decrement accumulator\n");
				break;
			case (dex):
				printf("dex - decrement x index\n");
				break;
			case (dey):
				printf("dey - decrement y index\n");
				break;
			case (inc):
				printf("inc - increment memory location\n");
				break;
			case (dec):
				printf("dec - decrement memory location\n");
				break;

			// shifts
			case (asl):
				printf("asl - arithmetic shift left, high bit into carry\n");
				break;
			case (lsr):
				printf("lsr - logical shift right, low bit into carry\n");
				break;
			case (rol):
				printf("rol - rotate left through carry\n");
				break;
			case (ror):
				printf("ror - rotate right through carry\n");
				break;

			// logical operations
			case (and):
				printf("and - AND accumulator\n");
				break;
			case (ora):
				printf("ora - OR accumulator\n");
				break;
			case (eor):
				printf("eor - XOR accumulator\n");
				break;
			case (bit):
				printf("bit - test bits against accumulator\n");
				break;
			case (cmp):
				printf("cmp - compare with accumulator\n");
				break;
			case (cpx):
				printf("cpx - compare with x index\n");
				break;
			case (cpy):
				printf("cpy - compare with y index\n");
				break;
			case (trb):
				printf("trb - test and reset bits\n");
				break;
			case (tsb):
				printf("tsb - test and set bits\n");
				break;
			case (rmb):
				printf("rmb - reset memory bit\n");
				break;
			case (smb):
				printf("smb - reset memory bit\n");
				break;

			// math
			case (adc):
				printf("adc - add accumultor with carry\n");
				break;
			case (smb):
				printf("smb - subtract accumulator with borrow\n");
				break;

			// flow control
			case (jmp):
				printf("jmp - unconditional jump\n");
				break;
			case (jsr):
				printf("jsr - jump subroutine\n");
				break;
			case (rts):
				printf("rts - return from subroutine\n");
				break;
			case (rti):
				printf("rti - return from interrupt\n");
				break;
			case (bra):
				printf("bra - branch always\n");
				break;
			case (beq):
				printf("beq - branch if zero set\n");
				break;
			case (bne):
				printf("bne - branch if zero clear\n");
				break;
			case (bcc):
				printf("bcc - branch if carry clear\n");
				break;
			case (bcs):
				printf("bcs - branch if carry set\n");
				break;
			case (bvc):
				printf("bvc - branch if overflow clear\n");
				break;
			case (bvs):
				printf("bvs - branch if overflow set\n");
				break;
			case (bmi):
				printf("bmi - branch on minus\n");
				break;
			case (bpl):
				printf("bpl - branch on plus\n");
				break;
			case (bbr):
				printf("bbr - branch on bit reset (0)\n");
				break;
			case (bbs):
				printf("bbs - branch on bit set (1)\n");
				break;

			// processor status
			case (clc):
				printf("clc - clear carry\n");
				break;
			case (cld):
				printf("cld - clear decimal mode\n");
				break;
			case (cli):
				printf("cli - clear interrupt disable bit\n");
				break;
			case (clv):
				printf("clv - clear overflow flag\n");
				break;
			case (sec):
				printf("sec - set carry flag\n");
				break;
			case (sed):
				printf("sed - set decimal mode\n");
				break;
			case (sei):
				printf("sei - set interrupt disable\n");
				break;

			// transfer instructions
			case (tax):
				printf("tax - transfer accumulator into x index\n");
				break;
			case (tay):
				printf("tay - transfer accumulator to y index\n");
				break;
			case (txa):
				printf("txa - transfer x index into accumulator\n");
				break;
			case (tya):
				printf("tya - transfer y index into accumulator\n");
				break;

			// other stuff
			case (nop):
				printf("nop\n");
				break;
			case (brk):
				printf("brk - force break\n");
				break;
		*/
		}

		// handle interrupts
		if (cycles_until_interrupt <= 0)
		{
			printf("Check for interrupt");
		}

	}
}