#pragma once
#include <msp430.h>
#include <stdint.h>
#include "flash_operations.h"

//-------------------------------------------------------------------//
// flash_statistics.h
//-------------------------------------------------------------------//
// 
//-------------------------------------------------------------------//
#define FS_PARTIAL_WRITE_FAIL 0xFFFF
#define FS_PARTIAL_ERASE_FAIL 0xFFFF

typedef struct fs_stats_struct {
  unsigned int incorrect_bit_count; // bits that are not the value expected
  unsigned int unstable_bit_count; // bits that change atleast once in 11 reads
  unsigned int write_latency; // latency for a proper word write
  unsigned int erase_latency; // latency for proper segment erase
  unsigned int partial_write_latency;
  unsigned int partial_erase_latency;
} fs_stats_s;

void fs_check_bit_values(f_segment_t seg, fs_stats_s* stats, uint16_t expected_val);
/*
  Function to get the number of incorrect bits and unstable bits in a segment

  incorrect bit - A bit in flash that differs from expected_val parameters
  unstable bit - Bit that reads differently atleast once out of STAT_READ_COUNT times
*/

void fs_get_partial_write_stats(uint16_t* target, fs_stats_s* stats, uint16_t val);
/*
  Function to get the fastest partial word write possible for a flash segment
  Tests 12, 10, 8, 6, 4, 0 NOP delayed partial word writes
*/

void fs_get_partial_erase_stats(f_segment_t seg, fs_stats_s* stats);
/*
  Function to get the fastest partial segment erase possible for a flash segment
  Tests 12, 10, 8, 6, 4, 0 ms delayed partial erases
*/
