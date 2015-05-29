#include <stdint.h>
#include "io.h"
#include "machine.h"

#define SET_CARRY(x)   x | 0b00100001
#define CLEAR_CARRY(x) x & 0b11111110
#define SET_ZERO(x)    x | 0b00100010
#define CLEAR_ZERO(x)  x & 0b11111101
#define SET_OVERFLOW(x) x | 0b01100000
#define CLEAR_OVERFLOW(x) x & 0b10111111
#define SET_NEG(x) x | 0b10100000
#define CLEAR_NEG(x) x & 0b01111111
#define SET_INTERRUPT(x) x | 0b00100100
#define CLEAR_INTERRUPT(x) x & 0b11111011

/*
* ADD with carry - Add value to dest and update flags.
* Flags affected: S, V, Z, C
* Cycles: 
*/
void adc(uint8_t value, uint8_t* dest, uint8_t* P)
{
	uint8_t sum = *dest + value;
	uint8_t carry = sum ^ (*dest) ^ value;

	// check for carry
	if (carry & 0b10000000)
		*P = SET_CARRY(*P); // set bit 0 = carry to 1
	else
		*P = CLEAR_CARRY(*P);

	// check for zero
	if (sum == 0b00000000)
		*P = SET_ZERO(*P);
	else
		*P = CLEAR_ZERO(*P);

	// check for overflow
	if ((sum + carry) & 0b10000000)
		*P = SET_OVERFLOW(*P);
	else
		*P = CLEAR_OVERFLOW(*P);

	// check for negative
	if (sum & 0b10000000)
		SET_NEG(*P);
	else
		CLEAR_NEG(*P);

	*dest = sum;
}

/*
* ADD with carry for 16 bit values. Helper function that lets us set P accordingly
* for indirect addressing.
*/
void adc_16(uint8_t value, uint16_t* dest, uint8_t* P)
{
	uint8_t sum = *dest + value;
	uint8_t carry = sum ^ (*dest) ^ value;

	// check for carry
	if (carry & 0x800)
		*P = SET_CARRY(*P); // set bit 0 = carry to 1
	else
		*P = CLEAR_CARRY(*P);

	// check for zero
	if (sum == 0x00)
		*P = SET_ZERO(*P);
	else
		*P = CLEAR_ZERO(*P);

	// check for overflow
	if ((sum + carry) & 0x800)
		*P = SET_OVERFLOW(*P);
	else
		*P = CLEAR_OVERFLOW(*P);

	// check for negative
	if (sum & 0b10000000)
		SET_NEG(*P);
	else
		CLEAR_NEG(*P);

	*dest = sum;
}


/* page_check - helper function that checks if addr1 and 2 are on the same page
*	returns 1 if they are on the same page
*	returns 0 if they are on different pages
*	returns 2 if one lies on a page boundary
* page size = 256 bytes. => 
*/
char page_check(uint16_t addr1, uint16_t addr2)
{
	if (((uint32_t)addr1 - (uint32_t)addr2) > 0xFFFF || ((uint32_t)addr2 - (uint32_t)addr1) > 0xFFFF) {
		return 0;
	} else if  (((uint32_t)addr1 - (uint32_t)addr2) == -1 || ((uint32_t)addr2 - (uint32_t)addr1) == -1) {
		return 2;
	} else {
		return 1;
	}
}

/* Return an indirect address offset by X */
uint16_t indx_address(uint8_t top, uint8_t bot, machine* mch)
{
	adc(mch->X, &top, &(mch->P));
	adc(mch->X, &bot, &(mch->P));
	top = mch->memory[top];
	bot = mch->memory[bot];
	uint16_t address = (((uint16_t)top << 8) | bot);
	return address;
}

/* Return an indirect address offset by Y */
uint16_t indy_address(uint8_t top, uint8_t bot, machine* mch)
{
	top = mch->memory[top];
	bot = mch->memory[bot];
	uint16_t address = ((top << 8) | bot);
	adc_16(mch->Y, &address, &(mch->P));
	return address;
}

/* Branch - branch depending on if the value specified in bit is set. */
void branch_set(uint8_t high, uint8_t low, machine* mch, int8_t bit)
{
	printf("branch\n");
	// is the flag specified in "bit" set?
	if ((mch->P & bit) != 0) {
		// evaluate address
		uint16_t address = (high << 8) | low;
		if (page_check(address, mch->pc) != 1) {
			mch->cycle += 1;
		}
		mch->pc = address - 1;
		mch->cycle += 1;
	}

	mch->cycle += 2;
}

/* Branch - branch depending on if the value specified in bit is clear. */
void branch_clear(uint8_t high, uint8_t low, machine* mch, int8_t bit)
{
	printf("branch\n");
	// is the flag specified in "bit" clear?
	if ((mch->P & bit) == 0) {
		// evaluate address
		uint16_t address = (high << 8) | low;
		if (page_check(address, mch->pc) != 1) {
			mch->cycle += 1;
		}
		mch->pc = address - 1;
		mch->cycle += 1;
	}

	mch->cycle += 2;
}


/* NOP - do nothing */
void nop(machine* mch)
{
	printf("nop\n");
	mch->cycle += 1;
}

/*
* CMP - compare value with accumulator
* Z = 1 if value == mch, 0 otherwise
* N = 1 if accumulator < value
* N = 0 if accumulator > value
*/

/* immediate addressing */
void cmp_imm(uint8_t value, machine* mch)
{

	uint8_t cmp = mch->A - value;

	if (cmp == 0) {
		mch->P = SET_ZERO(mch->P);
	} else if (cmp > 0) {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = CLEAR_NEG(mch->P);
	} else {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = SET_NEG(mch->P);
	}

	mch->pc += 1;
	mch->cycle += 2;
}

void cmp_zp(uint8_t address, machine* mch)
{
	uint8_t cmp = mch->A - mch->memory[address];
	if (cmp == 0) {
		mch->P = SET_ZERO(mch->P);
	} else if (cmp > 0) {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = CLEAR_NEG(mch->P);
	} else {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = SET_NEG(mch->P);
	}

	mch->pc += 1;
	mch->cycle += 3;
}

void cmp_zpx(uint8_t address, machine* mch)
{
	uint16_t adr = (uint16_t) address;
	adc_16(mch->X, &adr, &(mch->P));
	uint8_t cmp = mch->A - mch->memory[adr];

	if (cmp == 0) {
		mch->P = SET_ZERO(mch->P);
	} else if (cmp > 0) {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = CLEAR_NEG(mch->P);
	} else {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = SET_NEG(mch->P);
	}

	mch->pc += 1;
	mch->cycle += 4;
}

void cmp_abs(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t adr = ((uint16_t) high << 8) | low;
	uint8_t cmp = mch->A - mch->memory[adr];

	if (cmp == 0) {
		mch->P = SET_ZERO(mch->P);
	} else if (cmp > 0) {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = CLEAR_NEG(mch->P);
	} else {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = SET_NEG(mch->P);
	}

	mch->pc += 2;
	mch->cycle += 4;
}

void cmp_absx(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t adr = ((uint16_t) high << 8) | low;
	adc_16(mch->X, &adr, &(mch->P));
	uint8_t cmp = mch->A - mch->memory[adr];

	if (cmp == 0) {
		mch->P = SET_ZERO(mch->P);
	} else if (cmp > 0) {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = CLEAR_NEG(mch->P);
	} else {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = SET_NEG(mch->P);
	}

	if (page_check(adr, mch->P) != 1){
		mch->cycle += 1;
	}

	mch->pc += 2;
	mch->cycle += 4;
}

/*
* CPX - compare value with X
* Z = 1 if value == mch, 0 otherwise
* N = 1 if X < value
* N = 0 if X > value
*/

/* immediate addressing */
void cpx_imm(uint8_t value, machine* mch)
{
	uint8_t cmp = mch->X - value;

	if (cmp == 0) {
		mch->P = SET_ZERO(mch->P);
	} else if (cmp > 0) {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = CLEAR_NEG(mch->P);
	} else {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = SET_NEG(mch->P);
	}

	mch->pc += 1;
	mch->cycle += 2;
}

/*
* CPY - compare value with Y
* Z = 1 if value == mch, 0 otherwise
* N = 1 if Y < value
* N = 0 if Y > value
*/

/* immediate addressing */
void cpy_imm(uint8_t value, machine* mch)
{
	uint8_t cmp = mch->Y - value;

	if (cmp == 0) {
		mch->P = SET_ZERO(mch->P);
	} else if (cmp > 0) {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = CLEAR_NEG(mch->P);
	} else {
		mch->P = CLEAR_ZERO(mch->P);
		mch->P = SET_NEG(mch->P);
	}

	mch->pc += 1;
	mch->cycle += 2;
}

/*
* DEX - Decrement X
* Flags affected - Z, N
*/
void dex(machine* mch)
{
	uint8_t dec = mch->X - 1;

	if (dec < 0)
		mch->P = SET_NEG(mch->P);
	else
		mch->P = CLEAR_NEG(mch->P);

	if (dec == 0)
		mch->P = SET_ZERO(mch->P);
	else
		mch->P = CLEAR_ZERO(mch->P);

	mch->X = dec;
	mch->cycle += 2;
}

/*
* DEX - Decrement Y
* Flags affected - Z, N
*/
void dey(machine* mch)
{
	uint8_t dec = mch->Y - 1;

	if (dec < 0)
		mch->P = SET_NEG(mch->P);
	else
		mch->P = CLEAR_NEG(mch->P);

	if (dec == 0)
		mch->P = SET_ZERO(mch->P);
	else
		mch->P = CLEAR_ZERO(mch->P);

	mch->Y = dec;
	mch->cycle += 2;
}

/* CLC - clear carry */
void clc(machine* mch)
{
	mch->P = CLEAR_CARRY(mch->P);
	mch->cycle += 2;
}

 /* CLV - clear overflow */
void clv(machine* mch)
{
	mch->P = CLEAR_OVERFLOW(mch->P);
	mch->cycle += 2;
}

/* CLI - clear interrupt */
void cli(machine* mch)
{
	mch->P = CLEAR_INTERRUPT(mch->P);
	mch->cycle += 2;
}

/*
* AND - Bitwise AND with accumulator
* Flags affected: S, Z
*/
void and(uint8_t value, uint8_t* A, uint8_t* P)
{
	uint8_t anded = *A & value;

	// check if result is 0
	if (anded == 0b00000000)
		*P = SET_ZERO(*P);
	else
		*P = CLEAR_ZERO(*P);

	// check if result is negative or positive
	if (anded & 0b10000000)
		*P = SET_NEG(*P);
	else
		*P = CLEAR_NEG(*P);
}

/* immediate addressing */
void adc_imm(uint8_t value, machine* mch)
{
	adc(value, &(mch->A), &(mch->P));
	mch->pc += 1;
	mch->cycle += 2;
}

/* zero page addressing */
void adc_zp(uint8_t address, machine* mch)
{
	address %= 256;
	adc(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 1;
	mch->cycle += 2;
}

/* zero page, offset by X */
void adc_zpx(uint8_t address, machine* mch)
{
	adc(mch->X, &address, &(mch->P));
	address %= 256;
	adc(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 1;
	mch->cycle += 2;
}

/* absolute addressing */
void adc_abs(uint8_t high, uint8_t low, machine* mch)
{	
	uint16_t address = (high << 8) | low;
	adc(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 3;
}

/* absolute addressing, offset by the value in X */
void adc_absx(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	adc_16(mch->X, &address, &(mch->P)); // add with carry X to opcode
	adc(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 3;
}

/* absolute addressing, offset by the value in Y */
void adc_absy(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	adc_16(mch->Y, &address, &(mch->P)); // add with carry Y to opcode
	adc(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 3;
}

/* indirect addressing, offset by the value of X */
void adc_indx(uint8_t top, uint8_t bot, machine* mch)
{
	adc(mch->X, &top, &(mch->P));
	adc(mch->X, &bot, &(mch->P));
	top = mch->memory[top];
	bot = mch->memory[bot];
	uint16_t address = (((uint16_t)top << 8) | bot);
	adc(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 2;
}

/* indirect addressing, offset by the value of Y */
void adc_indy(uint8_t top, uint8_t bot, machine* mch)
{
	top = mch->memory[top];
	bot = mch->memory[bot];
	uint16_t address = ((top << 8) | bot);
	adc_16(mch->Y, &address, &(mch->P));
	adc(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 2;
}

/* immediate addressing */
void and_imm(uint8_t value, machine* mch)
{
	and(value, &(mch->A), &(mch->P));
	mch->pc += 1;
	mch->cycle += 2;
}

/* zero page */
void and_zp(uint8_t address, machine* mch)
{
	and(mch->memory[address%256], &(mch->A), &(mch->P));
	mch->cycle += 3;
	mch->pc += 1;
}

/* zero page offset by x */
void and_zpx(uint8_t address, machine* mch)
{
	adc(mch->X, &address, &(mch->P));
	address %= 256;
	and(mch->memory[address], &(mch->A), &(mch->P));
	mch->cycle += 4;
	mch->pc += 1;
}

/* absolute */
void and_abs(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	and(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 4;
}

/* absolute offset by x */
void and_absx(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	adc_16(mch->X, &address, &(mch->P));
	and(mch->memory[address], &(mch->A), &(mch->P));

	if (page_check(address, mch->pc) != 1) {
		mch->cycle += 1;
	}

	mch->pc += 2;
	mch->cycle += 4;
}

/* absolute offest by y */
void and_absy(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	adc_16(mch->Y, &address, &(mch->P));
	and(mch->memory[address], &(mch->A), &(mch->P));

	if (page_check(address, mch->pc) != 1) {
		mch->cycle += 1;
	}

	mch->pc += 2;
	mch->cycle += 4;
}

/* indirect offset by x */
void and_indx(uint8_t top, uint8_t bot, machine* mch)
{
	adc(mch->X, &top, &(mch->P));
	adc(mch->X, &bot, &(mch->P));
	top = mch->memory[top];
	bot = mch->memory[bot];
	uint16_t address = (((uint16_t)top << 8) | bot);
	and(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 6;
}

/* indirect offset by y*/
void and_indy(uint8_t top, uint8_t bot, machine* mch)
{
	top = mch->memory[top];
	bot = mch->memory[bot];
	uint16_t address = (((uint16_t)top << 8) | bot);
	adc_16(mch->Y, &address, &(mch->P));
	and(mch->memory[address], &(mch->A), &(mch->P));

	if (page_check(address, mch->pc) != 1) {
		mch->cycle += 1;
	}

	mch->pc += 2;
	mch->cycle += 5;
}

/*
* ORA - Bitwise OR with accumulator
* Flags affected: S, Z
*/
void or(uint8_t value, uint8_t* A, uint8_t* P)
{
	uint8_t ored = *A | value;

	// check if result is 0
	if (ored == 0b00000000)
		*P = SET_ZERO(*P);
	else
		*P = CLEAR_ZERO(*P);

	// check if result is negative or positive
	if (ored & 0b10000000)
		*P = SET_NEG(*P);
	else
		*P = CLEAR_NEG(*P);
}

/* immediate addressing */
void or_imm(uint8_t value, machine* mch)
{
	or(value, &(mch->A), &(mch->P));
	mch->pc += 1;
	mch->cycle += 2;
}

/* zero page */
void or_zp(uint8_t address, machine* mch)
{
	or(mch->memory[address%256], &(mch->A), &(mch->P));
	mch->cycle += 3;
	mch->pc += 1;
}

/* zero page offset by x */
void or_zpx(uint8_t address, machine* mch)
{
	adc(mch->X, &address, &(mch->P));
	address %= 256;
	or(mch->memory[address], &(mch->A), &(mch->P));
	mch->cycle += 4;
	mch->pc += 1;
}

/* absolute */
void or_abs(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	or(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 4;
}

/* absolute offset by x */
void or_absx(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	adc_16(mch->X, &address, &(mch->P));
	or(mch->memory[address], &(mch->A), &(mch->P));

	if (page_check(address, mch->pc) != 1) {
		mch->cycle += 1;
	}

	mch->pc += 2;
	mch->cycle += 4;
}

/* absolute offest by y */
void or_absy(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	adc_16(mch->Y, &address, &(mch->P));
	or(mch->memory[address], &(mch->A), &(mch->P));

	if (page_check(address, mch->pc) != 1) {
		mch->cycle += 1;
	}

	mch->pc += 2;
	mch->cycle += 4;
}

/* indirect offset by x */
void or_indx(uint8_t top, uint8_t bot, machine* mch)
{
	adc(mch->X, &top, &(mch->P));
	adc(mch->X, &bot, &(mch->P));
	top = mch->memory[top];
	bot = mch->memory[bot];
	uint16_t address = (((uint16_t)top << 8) | bot);
	or(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 6;
}

/* indirect offset by y*/
void or_indy(uint8_t top, uint8_t bot, machine* mch)
{
	top = mch->memory[top];
	bot = mch->memory[bot];
	uint16_t address = (((uint16_t)top << 8) | bot);
	adc_16(mch->Y, &address, &(mch->P));
	or(mch->memory[address], &(mch->A), &(mch->P));

	if (page_check(address, mch->pc) != 1) {
		mch->cycle += 1;
	}

	mch->pc += 2;
	mch->cycle += 5;
}


/*
* ASL - Arithmetic shift left dest by value bits.
* Flags affected: N, Z, C
*/
void asl(uint8_t value, uint8_t* dest, uint8_t* P)
{
	// negative left shifts aren't defined in C
	if (*dest >= 0) {
		*dest <<= value;
	} else {
		*dest <<= value;
		*dest |= 0b10000000;
	}

	// set the zero flag
	if (*dest == 0)
		*P = SET_ZERO(*P);
	else
		*P = CLEAR_ZERO(*P);

	// set the carry flag
	if ((0b00000001 << value) == 0)
		*P = SET_CARRY(*P);
	else
		*P = CLEAR_CARRY(*P);

	// set the neg flag
	if (*dest < 0)
		*P = SET_NEG(*P);
	else
		*P = CLEAR_NEG(*P);
}

void asl_imm(uint8_t value, machine* mch)
{
	asl(value, &(mch->A), &(mch->P));
	mch->pc += 1;
	mch->cycle += 2;
}

void asl_zp(uint8_t address, machine* mch)
{	
	asl(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 1;
	mch->cycle += 5;
}

void asl_zpx(uint8_t address, machine* mch)
{	
	uint16_t adr = address;
	adc_16(mch->X, &adr, &(mch->P));
	asl(mch->memory[adr], &(mch->A), &(mch->P));
	mch->pc += 1;
	mch->cycle += 6;
}

void asl_abs(uint8_t top, uint8_t bot, machine* mch)
{	
	uint16_t address = ((uint16_t) mch->memory[top] << 8) | mch->memory[bot];
	asl(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 6;
}

void asl_absx(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	adc_16(mch->X, &address, &(mch->P));
	asl(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 7;
}

/* JMP - set PC to given address */
void jmp(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	mch->pc = address-1;
	mch->cycle += 3;
}

/* BIT - check if the bits in A are set in the value at some memory address
* Only impacts zero flag. Does not store a result. 
*/
void bit(uint8_t pattern, uint8_t value, uint8_t* P)
{
	// one or more bits set?
	if ((pattern & value) != 0) {
		*P = CLEAR_ZERO(*P);
	} else {
		*P = SET_ZERO(*P);
	}

}

void bit_zp(uint8_t pat_adr, machine* mch)
{
	bit(mch->A, mch->memory[pat_adr], &(mch->P));
	mch->cycle += 3;
	mch->pc += 1;
}

void bit_abs(uint8_t top, uint8_t bot, machine* mch)
{
	uint16_t adr = ((uint16_t)top << 8) | bot;
	bit(mch->A, mch->memory[adr], &(mch->P));
	mch->cycle += 4;
	mch->pc += 2;
}

/* DEC - decrement value at specified memory location by 1
* Impacts zero and negative flags
*/

void dec(uint8_t* value, uint8_t* P)
{
	*value -= 1;
	if (*value == 0) {
		*P = SET_ZERO(*P);
	}

	if (*value < 0) {
		*P = SET_NEG(*P);
	}
}

void dec_zp(uint8_t address, machine* mch)
{	
	dec(&(mch->memory[address]), &(mch->P));
	mch->cycle += 5;
	mch->pc += 1;
}

void dec_zpx(uint8_t address, machine* mch)
{	
	uint16_t adr = (uint16_t) address;
	adc_16(mch->X, &adr, &(mch->P));
	dec(&(mch->memory[adr]), &(mch->P));
	mch->cycle += 6;
	mch->pc += 1;
}

void dec_abs(uint8_t top, uint8_t bot, machine* mch)
{	
	uint16_t adr = ((uint16_t) top << 8) | bot;
	dec(&(mch->memory[adr]), &(mch->P));
	mch->cycle += 6;
	mch->pc += 2;
}

void dec_absx(uint8_t top, uint8_t bot, machine* mch)
{	
	uint16_t adr = ((uint16_t) top << 8) | bot;
	adc_16(mch->X, &adr, &(mch->P));
	dec(&(mch->memory[adr]), &(mch->P));
	mch->cycle += 7;
	mch->pc += 2;
}


