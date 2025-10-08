#include "flash_operations.h"
#include <msp430.h>
#include <stdint.h>
#include "event_timer.h"
#include "SRAM_subroutine_copy.h"

#define BANK_SEGMENT_SIZE 512
#define INFO_SEGMENT_SIZE 128

void f_segment_erase(uint16_t* segPtr)
{
  while(FCTL3 & BUSY);

  FCTL3 = FWPW; //clear lock
  FCTL1 = FWPW + ERASE; // enable segment erase
  *segPtr = 0x0000; // dummy write to initiate erase

  while(FCTL3 & BUSY); // loop while busy
  // not really necessary when executing from flash

  FCTL1 = FWPW; // clear ERASE
  FCTL3 = FWPW + LOCK; // lock
}


void f_segment_erase_timed(uint16_t* segPtr)
{
  EVENT_TIMER_START; // defined in event_timer.h
  while(FCTL3 & BUSY);

  FCTL3 = FWPW; //clear lock
  FCTL1 = FWPW + ERASE; // enable segment erase
  *segPtr = 0x0000; // dummy write to initiate erase

  while(FCTL3 & BUSY); // loop while busy
  // not really necessary when executing from flash

  FCTL1 = FWPW; // clear ERASE
  FCTL3 = FWPW + LOCK; // lock
  EVENT_TIMER_STOP;
}

void f_bank_erase(uint16_t* bankPtr)
{
  while(FCTL3 & BUSY);

  FCTL3 = FWPW; //clear lock
  FCTL1 = FWPW + MERAS; // enable bank erase
  *bankPtr = 0x0000; // dummy write to initiate erase

  while(FCTL3 & BUSY); // loop while busy
  // not really necessary when executing from flash

  FCTL1 = FWPW; // clear MERASE
  FCTL3 = FWPW + LOCK; // lock
}

void f_bank_erase_timed(uint16_t* bankPtr)
{
  EVENT_TIMER_START;
  while(FCTL3 & BUSY);

  FCTL3 = FWPW; //clear lock
  FCTL1 = FWPW + MERAS; // enable bank erase
  *bankPtr = 0x0000; // dummy write to initiate erase

  while(FCTL3 & BUSY); // loop while busy
  // not really necessary when executing from flash

  FCTL1 = FWPW; // clear MERASE
  FCTL3 = FWPW + LOCK; // lock
  EVENT_TIMER_STOP;
}

void f_word_write(uint16_t value, uint16_t* targetPtr)
{
  while(FCTL3 & BUSY);

  FCTL3 = FWPW; //clear lock
  FCTL1 = FWPW + WRT; // enable word write
  *targetPtr = value; // write value

  while(FCTL3 & BUSY);

  FCTL1 = FWPW; // clear WRT
  FCTL3 = FWPW + LOCK; // lock
}

void f_word_write_timed(uint16_t value, uint16_t* targetPtr)
{
  EVENT_TIMER_START;
  while(FCTL3 & BUSY);


  FCTL3 = FWPW; //clear lock
  FCTL1 = FWPW + WRT; // enable word write
  *targetPtr = value; // write value

  while(FCTL3 & BUSY);

  FCTL1 = FWPW; // clear WRT
  FCTL3 = FWPW + LOCK; // lock
  EVENT_TIMER_STOP;
}


void f_safe_word_write(uint16_t value, uint16_t* targetPtr, f_segment_t seg, uint16_t segSize)
/*
   Function to write a word to a spot in flash memory without invalidating
   areas around it.
   Copies every value in the sector into the heap in order to restore after.
*/
{
  uint16_t* savedArray = (uint16_t*)malloc(segSize); // allocates LOTS of memory
  // most of the time allocates 512 bytes!
  segSize = segSize >> 1; //convert to words

  for(int i = 0; i < segSize; i++) // copy items over to RAM
    savedArray[i] = ((uint16_t*)seg)[i];

  savedArray[(targetPtr - seg) >> 1] = value;
  // place new value

  f_segment_erase((uint16_t*)seg);

  for(int i = 0; i < segSize; i++) // write values
    f_word_write(savedArray[i], (uint16_t*)seg + i);

  free(savedArray); // deallocate memory
}


void f_block_set(uint16_t value, uint16_t* blockPtr)
// magic numbers for a block size of 512
// Only a 128 byte row can be written at once
// must be executed from RAM
{
  while(FCTL3 & BUSY);

  FCTL3 = FWPW; // clear lock

  for(uint8_t s = 4; s != 0; s--){
    FCTL1 = FWPW + WRT + BLKWRT;

    for(int i = 32; i != 0; i--){
      *(blockPtr++) = value;
      *(blockPtr++) = value;
      while(!(FCTL3 & WAIT));
    }

    FCTL1 = FWPW + WRT; // clear BLKWRT
    while(FCTL3 & BUSY);
  }

  FCTL1 = FWPW; // clear BLKWRT and WRT
  FCTL3 = FWPW + LOCK; // lock
}
void end_f_block_set(void) {}


void f_segment_partial_erase_4(uint16_t* targetPtr)
// THIS FUNCTION MUST BE EXECUTED FROM RAM
// segment erase takes a very long time 23 - 32 ms for the F5529
// emergency exit after 10 us
{
  EVENT_TIMER_START;
  while(FCTL3 & BUSY);

  FCTL3 = FWPW;          // clear lock
  FCTL1 = FWPW + ERASE; // enable segment erase
  *targetPtr = 0x0000; // dummy write to initiate erase

  __no_operation(); // 4x NOP instructions
  __no_operation(); // ~4 us delay
  __no_operation();
  __no_operation();

  FCTL3 = FWPW + EMEX; // emergency exit
  FCTL1 = FWPW; // clear ERASE
  FCTL3 = FWPW + LOCK; // lock
  EVENT_TIMER_STOP;
}
void end_f_segment_partial_erase_4(void) {}


void f_segment_partial_erase_x(uint16_t* targetPtr, uint16_t x)
/*
  This function is the partial erase function with a timer delay
  x is the number of clock cycles to delay using the timer
  1.024 MHZ clock as input to the timer
 */
{
  while(FCTL3 & BUSY);
  EVENT_TIMER_START;

  FCTL3 = FWPW;          // clear lock
  FCTL1 = FWPW + ERASE; // enable segment erase
  *targetPtr = 0x0000; // dummy write to initiate erase

  //USE TIMER TO HALT UNTIL 10MS
  TA1CTL |= TACLR;
  TA1CTL |= TASSEL_2 + ID__1 + MC_2; // use SCLK
  while(TA1R < x);
  TA1CTL &= ~MC_3; // halt timer

  FCTL3 = FWPW + EMEX; // emergency exit
  FCTL1 = FWPW; // clear ERASE
  FCTL3 = FWPW + LOCK; // lock
  EVENT_TIMER_STOP;
}
void end_f_segment_partial_erase_x(void) {}


void f_word_partial_write_0(uint16_t partialValue, uint16_t* targetPtr)
// THIS FUNCTION MUST BE EXECUTED FROM RAM
// This function is timed!!! the value in _event_timer_value is the write time
{
  EVENT_TIMER_START;
  while(FCTL3 & BUSY);

  FCTL3 = FWPW; //clear lock
  FCTL1 = FWPW + WRT; // enable word write
  *targetPtr = partialValue; // write value

  FCTL3 = FWPW + EMEX; // emergency exit
  FCTL1 = FWPW; // clear WRT
  FCTL3 = FWPW + LOCK; // lock
  while(FCTL3 & BUSY);
  EVENT_TIMER_STOP;
}
void end_f_word_partial_write_0(void) {}

void f_word_partial_write_4(uint16_t partialValue, uint16_t* targetPtr)
{
  EVENT_TIMER_START;
  while(FCTL3 & BUSY);

  FCTL3 = FWPW; //clear lock
  FCTL1 = FWPW + WRT; // enable word write
  *targetPtr = partialValue; // write value

  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();

  FCTL3 = FWPW + EMEX; // emergency exit
  FCTL1 = FWPW; // clear WRT
  FCTL3 = FWPW + LOCK; // lock
  while(FCTL3 & BUSY);
  EVENT_TIMER_STOP;
}
void end_f_word_partial_write_4(void) {}

void f_word_partial_write_6(uint16_t partialValue, uint16_t* targetPtr)
{
  EVENT_TIMER_START;
  while(FCTL3 & BUSY);

  FCTL3 = FWPW; //clear lock
  FCTL1 = FWPW + WRT; // enable word write
  *targetPtr = partialValue; // write value

  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();

  FCTL3 = FWPW + EMEX; // emergency exit
  FCTL1 = FWPW; // clear WRT
  FCTL3 = FWPW + LOCK; // lock
  while(FCTL3 & BUSY);
  EVENT_TIMER_STOP;
}
void end_f_word_partial_write_6(void) {}

void f_word_partial_write_8(uint16_t partialValue, uint16_t* targetPtr)
// THIS FUNCTION MUST BE EXECUTED FROM RAM
// This function is timed!!! the value in _event_timer_value is the write time
{
  EVENT_TIMER_START;
  while(FCTL3 & BUSY);

  FCTL3 = FWPW; //clear lock
  FCTL1 = FWPW + WRT; // enable word write
  *targetPtr = partialValue; // write value

  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();

  FCTL3 = FWPW + EMEX; // emergency exit
  FCTL1 = FWPW; // clear WRT
  FCTL3 = FWPW + LOCK; // lock
  while(FCTL3 & BUSY);
  EVENT_TIMER_STOP;
}
void end_f_word_partial_write_8(void) {}

void f_word_partial_write_10(uint16_t partialValue, uint16_t* targetPtr)
// emergency exit after ~10 us
{
  EVENT_TIMER_START;
  while(FCTL3 & BUSY);

  FCTL3 = FWPW; //clear lock
  FCTL1 = FWPW + WRT; // enable word write
  *targetPtr = partialValue; // write value

  __no_operation(); // 10x NOP instructions
  __no_operation(); // ~10 us delay
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();


  FCTL3 = FWPW + EMEX; // emergency exit
  FCTL1 = FWPW; // clear WRT
  FCTL3 = FWPW + LOCK; // lock
  while(FCTL3 & BUSY);
  EVENT_TIMER_STOP;
}
void end_f_word_partial_write_10(void) {}

void f_word_partial_write_12(uint16_t partialValue, uint16_t* targetPtr)
// emergency exit after ~12 us
{
  EVENT_TIMER_START;
  while(FCTL3 & BUSY);

  FCTL3 = FWPW; //clear lock
  FCTL1 = FWPW + WRT; // enable word write
  *targetPtr = partialValue; // write value

  __no_operation(); // 10x NOP instructions
  __no_operation(); // ~10 us delay
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();


  FCTL3 = FWPW + EMEX; // emergency exit
  FCTL1 = FWPW; // clear WRT
  FCTL3 = FWPW + LOCK; // lock
  while(FCTL3 & BUSY);
  EVENT_TIMER_STOP;
}
void end_f_word_partial_write_12(void) {}


void f_stress_segment(f_segment_t seg, uint16_t val, uint32_t iterations)
// Since erasing flash forces all bits high, a value of 0x0000 will result in
//    the highest possible stresssing of all bits.
// LEAVES A SEGMENT WITH THE VALUE OF VAL IN EVERY WORD
{
  void (*SRAM_f_block_set)(uint16_t, uint16_t*); // declare function pointer

  SRAM_f_block_set = malloc_subroutine(f_block_set, end_f_block_set);
  if(!(void*)SRAM_f_block_set)
    return; // null pointer means the memory cannot be allocated

  for (uint32_t i = iterations; i != 0; i--){
    f_segment_erase((uint16_t*)seg);

    SRAM_f_block_set(val, (uint16_t*)seg);
  }

  free((void*)SRAM_f_block_set); // deallocate memory
}

void f_stress_bank(f_bank_t bank, uint16_t val, uint32_t iterations)
{
  f_segment_t target;
  void (*SRAM_f_block_set)(uint16_t, uint16_t*); // declare function pointer

  SRAM_f_block_set = malloc_subroutine(f_block_set, end_f_block_set);
  if(!(void*)SRAM_f_block_set)
    return; // null pointer means the memory cannot be allocated

  target = (f_segment_t)bank;

  for (uint32_t i = iterations; i != 0; i--){
    f_bank_erase((uint16_t*)bank);

    for(uint8_t s = F_BANK_N_SEGMENTS; s != 0; s--) // set all segments in the bank
      SRAM_f_block_set(val, (uint16_t*)(target++));
  }

  free((void*)SRAM_f_block_set); // deallocate memory
}
