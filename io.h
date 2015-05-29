#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include "machine.h"

// debugging routines
void print_bits(uint8_t x);
void print_bits16(uint16_t x);

// non debugging
uint8_t* read_rom (uint8_t* memory, FILE* fp);

#endif