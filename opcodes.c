#include <stdint.h>
#include "debug_io.h"
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

/* NOP - do nothing */
void nop(machine* mch)
{
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
	mch->cycle += 2;
}

/* zero page offset by x */
void and_zpx(uint8_t address, machine* mch)
{
	adc(mch->X, &address, &(mch->P));
	address %= 256;
	and(mch->memory[address], &(mch->A), &(mch->P));
	mch->cycle += 2;
}

/* absolute */
void and_abs(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	and(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 3;
}

/* absolute offset by x */
void and_absx(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	adc_16(mch->X, &address, &(mch->P));
	and(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 3;
}

/* absolute offest by y */
void and_absy(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	adc_16(mch->Y, &address, &(mch->P));
	and(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 3;
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
	mch->cycle += 2;
}

/* indirect offset by y*/
void and_indy(uint8_t top, uint8_t bot, machine* mch)
{
	top = mch->memory[top];
	bot = mch->memory[bot];
	uint16_t address = (((uint16_t)top << 8) | bot);
	adc_16(mch->Y, &address, &(mch->P));
	and(mch->memory[address], &(mch->A), &(mch->P));
	mch->pc += 2;
	mch->cycle += 2;
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

/* JMP - set PC to given address */
void jmp(uint8_t high, uint8_t low, machine* mch)
{
	uint16_t address = (high << 8) | low;
	mch->pc = address-1;
	mch->cycle += 3;
}

