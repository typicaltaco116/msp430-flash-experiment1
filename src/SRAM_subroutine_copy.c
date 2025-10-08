#include "SRAM_subroutine_copy.h"

void copy_subroutine(char* src_start, char* src_end, char* dst_start)
// src_start is the start address of the subroutine to copy
// src_end is the start address of the following function
// dst_start is the pointer to the destination
// USER MUST ENSURE THAT THE DESTINATION HAS ENOUGH ROOM
{
  while(src_start < src_end){
    *dst_start = *src_start;
    dst_start++;
    src_start++;
  }
}

void* malloc_subroutine(void* src_start, void* src_end)
/*
   This function is intended to allocate space on the heap and copy
   a subroutine into that space.
   Space should be deallocated with free() when no longer needed.
*/
{
  void* tempPtr = malloc(src_end - src_start);
  if (!tempPtr)
    return tempPtr; // not enough space in heap

  copy_subroutine((char*)src_start, (char*)src_end, tempPtr);

  return (void*)tempPtr;
}
