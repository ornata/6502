#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>
#include "io.h"
#include "machine.h"

void nop(machine* mch);
void clv(machine* m);
void cli(machine* m);
void clc(machine* mch);
void dex(machine* mch);
void dey(machine* mch);

void adc(uint8_t value, uint8_t* dest, uint8_t* P);
void adc_16(uint8_t value, uint16_t* dest, uint8_t* P);
void adc_imm(uint8_t value, machine* mch);
void adc_zp(uint8_t address, machine* mch);
void adc_zpx(uint8_t address, machine* mch);
void adc_abs(uint8_t high, uint8_t low, machine* mch);
void adc_absx(uint8_t high, uint8_t low, machine* mch);
void adc_absy(uint8_t high, uint8_t low, machine* mch);
void adc_indx(uint8_t top, uint8_t bot, machine* mch);
void adc_indy(uint8_t top, uint8_t bot, machine* mch);

void and(uint8_t value, uint8_t* A, uint8_t* P);
void and_imm(uint8_t value, machine* mch);
void and_zp(uint8_t address, machine* mch);
void and_zpx(uint8_t address, machine* mch);
void and_abs(uint8_t high, uint8_t low, machine* mch);
void and_absx(uint8_t high, uint8_t low, machine* mch);
void and_absy(uint8_t high, uint8_t low, machine* mch);
void and_indx(uint8_t top, uint8_t bot, machine* mch);
void and_indy(uint8_t top, uint8_t bot, machine* mch);

void asl(uint8_t value, uint8_t* dest, uint8_t* P);
void asl_imm(uint8_t value, machine* mch);
void asl_zp(uint8_t address, machine* mch);
void asl_zpx(uint8_t address, machine* mch);
void asl_abs(uint8_t top, uint8_t bot, machine* mch);
void asl_absx(uint8_t high, uint8_t low, machine* mch);

void branch_set(uint8_t high, uint8_t low, machine* mch, int8_t bit);
void branch_clear(uint8_t high, uint8_t low, machine* mch, int8_t bit);

void cmp_imm(uint8_t value, machine* mch);
void cpx_imm(uint8_t value, machine* mch);
void cpy_imm(uint8_t value, machine* mch);
void cmp_zp(uint8_t address, machine* mch);
void cmp_zpx(uint8_t address, machine* mch);
void cmp_abs(uint8_t high, uint8_t low, machine* mch);
void cmp_absx(uint8_t high, uint8_t low, machine* mch);

void or(uint8_t value, uint8_t* A, uint8_t* P);
void or_imm(uint8_t value, machine* mch);
void or_zp(uint8_t address, machine* mch);
void or_zpx(uint8_t address, machine* mch);
void or_abs(uint8_t high, uint8_t low, machine* mch);
void or_absx(uint8_t high, uint8_t low, machine* mch);
void or_absy(uint8_t high, uint8_t low, machine* mch);
void or_indx(uint8_t top, uint8_t bot, machine* mch);
void or_indy(uint8_t top, uint8_t bot, machine* mch);

char page_check(uint16_t addr1, uint16_t addr2);

void jmp(uint8_t high, uint8_t low, machine* mch);

#endif