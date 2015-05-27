#ifndef OPCODES_H
#define OPCODES_H

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

void cmp_imm(uint8_t value, machine* mch);
void cpx_imm(uint8_t value, machine* mch);
void cpy_imm(uint8_t value, machine* mch);

void jmp(uint8_t high, uint8_t low, machine* mch);

#endif