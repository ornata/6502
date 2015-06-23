#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>
#include "io.h"
#include "machine.h"

void nop(machine* mch, uint8_t cycles);
void clv(machine* m);
void cli(machine* m);
void clc(machine* mch);
void dex(machine* mch);
void dey(machine* mch);
void sei(machine* mch);
void php(machine* mch);
void plp(machine* mch);
void pha(machine* mch);
void pla(machine* mch);
void sec(machine* mch);

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

void asl(uint8_t* dest, uint8_t* P);
void asl_acc(machine* mch);
void asl_zp(uint8_t address, machine* mch);
void asl_zpx(uint8_t address, machine* mch);
void asl_abs(uint8_t top, uint8_t bot, machine* mch);
void asl_absx(uint8_t high, uint8_t low, machine* mch);

void bit(uint8_t pattern, uint8_t value, uint8_t* P);
void bit_zp(uint8_t pat_adr, machine* mch);
void bit_abs(uint8_t top, uint8_t bot, machine* mch);

void branch_set(uint8_t high, uint8_t low, machine* mch, int8_t bit);
void branch_clear(uint8_t high, uint8_t low, machine* mch, int8_t bit);

void brk(machine* mch);

void cmp_imm(uint8_t value, machine* mch);
void cpx_imm(uint8_t value, machine* mch);
void cpx_zp(uint8_t address, machine* mch);
void cpx_abs(uint8_t high, uint8_t low, machine* mch);
void cpy_imm(uint8_t value, machine* mch);
void cmp_zp(uint8_t address, machine* mch);
void cmp_zpx(uint8_t address, machine* mch);
void cmp_abs(uint8_t high, uint8_t low, machine* mch, char has_offset, uint8_t offset);
void cmp_indx(uint8_t high, uint8_t low, machine* mch);
void cmp_indy(uint8_t high, uint8_t low, machine* mch);

void dec(uint8_t* value, uint8_t* P);
void dec_zp(uint8_t address, machine* mch);
void dec_zpx(uint8_t address, machine* mch);
void dec_abs(uint8_t top, uint8_t bot, machine* mch);
void dec_absx(uint8_t top, uint8_t bot, machine* mch);

void eor(uint8_t value, machine* mch);
void eor_imm(uint8_t value, machine* mch);
void eor_zp(uint8_t value, machine* mch);
void eor_zpx(uint8_t value, machine* mch);
void eor_abs(uint8_t top, uint8_t bot, machine* mch);
void eor_absx(uint8_t top, uint8_t bot, machine* mch);
void eor_absy(uint8_t top, uint8_t bot, machine* mch);
void eor_indx(uint8_t top, uint8_t bot, machine* mch);
void eor_indy(uint8_t top, uint8_t bot, machine* mch);

void or(uint8_t value, uint8_t* A, uint8_t* P);
void or_imm(uint8_t value, machine* mch);
void or_zp(uint8_t address, machine* mch);
void or_zpx(uint8_t address, machine* mch);
void or_abs(uint8_t high, uint8_t low, machine* mch);
void or_absx(uint8_t high, uint8_t low, machine* mch);
void or_absy(uint8_t high, uint8_t low, machine* mch);
void or_indx(uint8_t top, uint8_t bot, machine* mch);
void or_indy(uint8_t top, uint8_t bot, machine* mch);

void lda_imm(uint8_t adr, machine* mch);
void lda_abs(uint8_t top, uint8_t bot, machine* mch, char has_offset, uint8_t offset);
void lda_zp(uint8_t adr, machine* mch, char has_offset, uint8_t offset);
void lda_indx(uint8_t top, uint8_t bot, machine* mch);
void lda_indy(uint8_t top, uint8_t bot, machine* mch);

void ldx_imm(uint8_t adr, machine* mch);
void ldx_zp(uint8_t adr, machine* mch, char has_offset, uint8_t offset);
void ldx_abs(uint8_t top, uint8_t bot, machine* mch, char has_offset, uint8_t offset);

void ldy_imm(uint8_t adr, machine* mch);
void ldy_zp(uint8_t adr, machine* mch, char has_offset, uint8_t offset);
void ldy_abs(uint8_t top, uint8_t bot, machine* mch, char has_offset, uint8_t offset);

void lsr_acc(machine* mch);
void lsr_zp(uint8_t address, machine* mch);
void lsr_zpx(uint8_t address, machine* mch);
void lsr_abs(uint8_t top, uint8_t bot, machine* mch);
void lsr_absx(uint8_t top, uint8_t bot, machine* mch);

char page_check(uint16_t addr1, uint16_t addr2);

void jmp(uint8_t high, uint8_t low, machine* mch);
void jmp_abs(uint8_t high, uint8_t low, machine* mch);
void jmp_ind(uint8_t high, uint8_t low, machine* mch);

void rts(machine* mch);
void rti(machine* mch);

void tax(machine* mch);
void tay(machine* mch);
void txa(machine* mch);
void tya(machine* mch);

#endif