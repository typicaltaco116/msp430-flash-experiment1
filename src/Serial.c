#include "Serial.h"


void Serial0_setup(void) 
// The Serial output from USCI_A1 is muxed with the JTAG interface over USB
{
    P4SEL |= BIT4 + BIT5;   // Set USCI_A1 RXD/TXD to receive/transmit data
    UCA1CTL1 |= UCSWRST;    // Set reset in order to modify control registers
    UCA1CTL0 &= 0x00;       // USCI_A1 control register
    UCA1CTL1 |= UCSSEL_2;   // Clock source SMCLK

    UCA1BR0 = 0x09;         // 1048576 Hz  / 115200 lower byte
    UCA1BR1 = 0x00;         // upper byte
    UCA1MCTL |= UCBRS0;     // Modulation (UCBRS0=0x01, UCOS16=0)
    UCA1CTL1 &= ~UCSWRST;   // Clear software reset to initialize USCI state machine
}

void Serial0_write(char* targetPtr)
//currently does not use interrupts for filling the transmit buffer
{
    while(*targetPtr) {
        while(!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *targetPtr++;
    }
}

void Serial0_put(char targetByte)
{
  while(!(UCA1IFG & UCTXIFG));
  UCA1TXBUF = targetByte;
}
