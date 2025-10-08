#pragma once
#include <msp430.h>
#include <stdlib.h>

void copy_subroutine(char* src_start, char* src_end, char* dst_start);

void* malloc_subroutine(void* src_start, void* src_end);
