#include <stdint.h>
#include "debug_io.h"

#define SET_CARRY(x)   x | 0b00010001
#define CLEAR_CARRY(x) x & 0b11111110
#define SET_ZERO(x)    x | 0b00010010
#define CLEAR_ZERO(x)  x & 0b11111101
#define SET_OVERFLOW(x) x | 0b01010000
#define CLEAR_OVERFLOW(x) x & 0b10111111
#define SET_NEG(x) x | 0b10010000
#define CLEAR_NEG(x) x & 0b01111111

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

