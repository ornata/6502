#ifndef OPCODES_H
#define OPCODES_H

#define SET_CARRY(x)   x | 0b00010001
#define CLEAR_CARRY(x) x & 0b11111110
#define SET_ZERO(x)    x | 0b00010010
#define CLEAR_ZERO(x)  x & 0b11111101
#define SET_OVERFLOW(x) x | 0b01010000
#define CLEAR_OVERFLOW(x) x & 0b10111111
#define SET_NEG(x) x | 0b10010000
#define CLEAR_NEG(x) x & 0b01111111

void adc(uint8_t value, uint8_t* dest, uint8_t* P);
void and(uint8_t value, uint8_t* A, uint8_t* P);
void asl(uint8_t value, uint8_t* dest, uint8_t* P);

#endif