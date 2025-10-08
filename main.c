/***************************************************************
* FILENAME: main.c
* DESCRIPTION:
* AUTHOR: Jack Pyburn
* DATE: 10/06/2025
* STATUS: PERFORMANCE ISSUES WITH STATISTICS GATHERING
****************************************************************/
#include <msp430.h> 
#include "src/flash_operations.h"
#include "src/flash_statistics.h"
#include "src/Serial.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define F5529_FLASH_BANK_D    0x1C400     /* FLASH BANK D starts at 0x1_C400 and ends at 0x2_43FF */
#define CHIP_ID_ADR           0x1A0A


#define TOTAL_PE_CYCLES       2000000 
#define STAT_INCREMENT_CYCLES 200000 // number of PE cycles to stress between stats
#define BUF_SIZE              64

void init_and_wait(void);
uint64_t get_chipID(void);


int main(void)
{
  char outputBuffer[BUF_SIZE];
  f_bank_t bank_D = (void*)F5529_FLASH_BANK_D;
  f_segment_t seg;
  fs_stats_s stats;

  WDTCTL = WDTPW + WDTHOLD;	// stop watchdog timer
  init_and_wait(); // holds program until user presses KEY1

  Serial0_setup();

  /* PRINT HEADER */
  Serial0_write("-------------------------------------------------------\n");
  Serial0_write("- Experiment 01\n");
  Serial0_write("- Purpose: Get statistics as flash wears to 2M cycles\n");
  sprintf(outputBuffer, "- Subject Chip ID: 0x%08llX\n", get_chipID());
  Serial0_write(outputBuffer);
  Serial0_write("-------------------------------------------------------\n");


  /* MAIN LOOP */
  for(uint32_t i = 0; i < TOTAL_PE_CYCLES / STAT_INCREMENT_CYCLES; i++){

    // stress bank
    f_stress_bank(bank_D, 0x0000, STAT_INCREMENT_CYCLES);
    /* 0x0000 indicates 100% flash bit wear
       f_stress_bank will return with all words written to 0x0000 */

    // print out number of cycles so far
    sprintf(outputBuffer, "\nCycle count: %lu\n\n", (uint32_t)((i + 1) * STAT_INCREMENT_CYCLES));
    Serial0_write(outputBuffer);

    seg = (f_segment_t)bank_D; // set to base segment

    // do statistics on every segment
    for(uint16_t s = 0 ; s < F_BANK_N_SEGMENTS; s++){
      sprintf(outputBuffer, "  Segment # %u Statistics\n", s);
      Serial0_write(outputBuffer);

      fs_check_bit_values(seg, &stats, 0x0000); // ~1500 ms
      f_segment_erase((uint16_t*)seg); // prepare segment for partial write testing
      fs_get_partial_write_stats((uint16_t*)seg, &stats, 0x0000);

      sprintf(outputBuffer, "    incorrect bit count   : %u\n", stats.incorrect_bit_count);
      Serial0_write(outputBuffer);
      sprintf(outputBuffer, "    unstable bit count    : %u\n", stats.unstable_bit_count);
      Serial0_write(outputBuffer);
      sprintf(outputBuffer, "    partial write latency : %u\n", stats.partial_write_latency);
      Serial0_write(outputBuffer);

      seg++;
    }
  }

  return 0;

}

void init_and_wait(void)
{
  P1REN |= BIT1;
  P1DIR |= BIT0;
  P1OUT |= BIT1 + BIT0;
  while(P1IN & BIT1);// wait till button press to start
  P1OUT &= ~BIT0;
}

uint64_t get_chipID(void)
{
  return *(uint64_t*)CHIP_ID_ADR;
}

