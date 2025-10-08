/***************************************************************
* FILENAME: main.c
* DESCRIPTION:
* AUTHOR: Jack Pyburn
* DATE: 10/06/2025
* STATUS:
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
#define STAT_INCREMENT_CYCLES 2000 // number of PE cycles to stress between stats
#define BUF_SIZE              64

void init_and_wait(void);
uint64_t get_chipID(void);


int main(void)
{
  char outputBuffer[BUF_SIZE];
  f_bank_t bank_D = (void*)F5529_FLASH_BANK_D;

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
    sprintf(outputBuffer, "\nCycle count: %lu\n", (uint32_t)(i * STAT_INCREMENT_CYCLES));
    Serial0_write(outputBuffer);

    // do statistics
    for(uint16_t s = 0 ; s < F_BANK_N_SEGMENTS; s++){
      sprintf(outputBuffer, "\n  Segment # %u Statistics\n", s);
      Serial0_write(outputBuffer);

    }

    // stress bank
    f_stress_bank(bank_D, 0x0000, 1); // 100% bit wear
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

