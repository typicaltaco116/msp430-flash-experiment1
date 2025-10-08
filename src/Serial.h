#pragma once
#include <msp430.h>

void Serial0_setup(void);

void Serial0_write(char* targetPtr);

void Serial0_put(char targetByte);
