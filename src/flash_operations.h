//-------------------------------------------------------------------//
// flash_operations.h
//-------------------------------------------------------------------//
// This header file contains functions for manipulating the flash
// memory on F5529 Lauchpad
// NOTES: 
// Intended for use with C11
// functions prepended with end are dummy functions for
//    determining the length of a function
// CODE_SECTION pragma is used to ensure that the functions are placed
//    sequentially
// SECTIONS MUST BE DEFINED IN LINKER COMMAND FILE
//-------------------------------------------------------------------//
#pragma once
#include <msp430.h>
#include <stdint.h>

#pragma CODE_SECTION(f_segment_partial_erase_4, ".f_segment_partial_erase_4")
#pragma CODE_SECTION(end_f_segment_partial_erase_4, ".f_segment_partial_erase_4")
#pragma CODE_SECTION(f_segment_partial_erase_x, ".f_segment_partial_erase_x")
#pragma CODE_SECTION(end_f_segment_partial_erase_x, ".f_segment_partial_erase_x")

#pragma CODE_SECTION(f_word_partial_write_0, ".f_word_partial_write_0")
#pragma CODE_SECTION(end_f_word_partial_write_0, ".f_word_partial_write_0")
#pragma CODE_SECTION(f_word_partial_write_4, ".f_word_partial_write_4")
#pragma CODE_SECTION(end_f_word_partial_write_4, ".f_word_partial_write_4")
#pragma CODE_SECTION(f_word_partial_write_6, ".f_word_partial_write_6")
#pragma CODE_SECTION(end_f_word_partial_write_6, ".f_word_partial_write_6")
#pragma CODE_SECTION(f_word_partial_write_8, ".f_word_partial_write_8")
#pragma CODE_SECTION(end_f_word_partial_write_8, ".f_word_partial_write_8")
#pragma CODE_SECTION(f_word_partial_write_10, ".f_word_partial_write_10")
#pragma CODE_SECTION(end_f_word_partial_write_10, ".f_word_partial_write_10")
#pragma CODE_SECTION(f_word_partial_write_12, ".f_word_partial_write_12")
#pragma CODE_SECTION(end_f_word_partial_write_12, ".f_word_partial_write_12")

#pragma CODE_SECTION(f_block_set, ".f_block_set")
#pragma CODE_SECTION(end_f_block_set, ".f_block_set")

#define F_BANK_N_SEGMENTS 64
#define F_SEGMENT_N_BYTES 512


// Both of these structures are not meant to be used as actual structures
// instead they will be used as pointers with custom increment amounts
// DO NOT INSTANTIATE ACTUAL STRUCTURE BECAUSE IT WILL TAKE UP 64K BYTES
typedef struct f_segment_struct {
  char bytesCount[512];
} *f_segment_t;

typedef struct f_bank_struct {
  f_segment_t segCount[64];
} *f_bank_t;

void f_segment_erase(uint16_t* segPtr);
void f_segment_erase_timed(uint16_t* segPtr);

void f_bank_erase(uint16_t* bankPtr);
void f_bank_erase_timed(uint16_t* bankPtr);

void f_word_write(uint16_t value, uint16_t* targetPtr);
void f_word_write_timed(uint16_t value, uint16_t* targetPtr);


void f_safe_word_write(uint16_t value, uint16_t* targetPtr, f_segment_t seg, uint16_t segSize);


void f_block_set(uint16_t value, uint16_t* blockPtr);
void end_f_block_set(void);


void f_segment_partial_erase_4(uint16_t* targetPtr);
void end_f_segment_partial_erase_4(void);

void f_segment_partial_erase_x(uint16_t* targetPtr, uint16_t x);
void end_f_segment_partial_erase_x(void);


void f_word_partial_write_0(uint16_t partialValue, uint16_t* targetPtr);
void end_f_word_partial_write_0(void);

void f_word_partial_write_4(uint16_t partialValue, uint16_t* targetPtr);
void end_f_word_partial_write_4(void);

void f_word_partial_write_6(uint16_t partialValue, uint16_t* targetPtr);
void end_f_word_partial_write_6(void);

void f_word_partial_write_8(uint16_t partialValue, uint16_t* targetPtr);
void end_f_word_partial_write_8(void);

void f_word_partial_write_10(uint16_t partialValue, uint16_t* targetPtr);
void end_f_word_partial_write_10(void);

void f_word_partial_write_12(uint16_t partialValue, uint16_t* targetPtr);
void end_f_word_partial_write_12(void);


void f_stress_segment(f_segment_t seg, uint16_t val, uint32_t iterations);

void f_stress_bank(f_bank_t bank, uint16_t val, uint32_t iterations);

