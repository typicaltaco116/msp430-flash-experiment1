#include "flash_statistics.h"
#include <msp430.h>
#include <stdint.h>
#include "flash_operations.h"
#include "event_timer.h"
#include "SRAM_subroutine_copy.h"


#define STAT_READ_COUNT 11

void fs_check_bit_values(f_segment_t seg, fs_stats_s* stats, uint16_t expected_val)
// majority based voting
// position of bits not set correctly
{
  uint16_t word_bin;
  uint16_t differences;
  uint16_t* read_head = (uint16_t*)seg;

  stats->incorrect_bit_count = 0;
  stats->unstable_bit_count = 0;
  
  while(read_head < (seg + 1)){

    // incorrect bit detection
    differences = *read_head ^ expected_val;
    for (int s = 16; s != 0; s--){
      if(differences & BIT0)
        stats->incorrect_bit_count++;
      differences = differences >> 1;
    }

    // unstable bit detection
    for (int i = 0; i < STAT_READ_COUNT; i++){
      word_bin = *read_head;
      differences = word_bin ^ *read_head;
      for (int s = 16; s != 0; s--){
        if(differences & BIT0)
          stats->unstable_bit_count++;
        differences = differences >> 1;
      }
    }

    read_head++;
  }

}

void fs_get_partial_write_stats(uint16_t* target, fs_stats_s* stats, uint16_t val)
{
  void (*SRAM_p_write)(uint16_t, uint16_t*); // declare function pointer 

  // Test 12 NOP delayed partial word write
  SRAM_p_write = malloc_subroutine(f_word_partial_write_12, end_f_word_partial_write_12);
  SRAM_p_write(val, target);
  free(SRAM_p_write);
  if (*target ^ val){
    stats->partial_write_latency = FS_PARTIAL_WRITE_FAIL;
    return;
  }
  stats->partial_write_latency = _event_timer_value;
  
  // Test 10 NOP delayed partial word write
  SRAM_p_write = malloc_subroutine(f_word_partial_write_10, end_f_word_partial_write_10);
  SRAM_p_write(val, target);
  free(SRAM_p_write);
  if (*target ^ val){
    stats->partial_write_latency = FS_PARTIAL_WRITE_FAIL;
    return;
  }
  stats->partial_write_latency = _event_timer_value;

  // Test 8 NOP delay partial word write
  SRAM_p_write = malloc_subroutine(f_word_partial_write_8, end_f_word_partial_write_8);
  SRAM_p_write(val, target);
  free(SRAM_p_write);
  if (*target ^ val){
    return;
  }
  stats->partial_write_latency = _event_timer_value;

  // Test 6 NOP delay partial word write
  SRAM_p_write = malloc_subroutine(f_word_partial_write_6, end_f_word_partial_write_6);
  SRAM_p_write(val, target);
  free(SRAM_p_write);
  if (*target ^ val){
    return;
  }
  stats->partial_write_latency = _event_timer_value;

  // Test 4 NOP delay partial word write
  SRAM_p_write = malloc_subroutine(f_word_partial_write_4, end_f_word_partial_write_4);
  SRAM_p_write(val, target);
  free(SRAM_p_write);
  if (*target ^ val){
    return;
  }
  stats->partial_write_latency = _event_timer_value;

  // Test 0 NOP delay partial word write
  SRAM_p_write = malloc_subroutine(f_word_partial_write_0, end_f_word_partial_write_0);
  SRAM_p_write(val, target);
  free(SRAM_p_write);
  if (*target ^ val){
    return;
  }
  stats->partial_write_latency = _event_timer_value;
}


void fs_get_partial_erase_stats(f_segment_t seg, fs_stats_s* stats)
{
  void (*SRAM_p_erase)(uint16_t*, uint16_t);
  
  // load this array with the delays to test partial segment operations
  const char testDelayArray[6] = {2, 4, 6, 8, 10, 12};

  SRAM_p_erase = malloc_subroutine(f_segment_partial_erase_x, \
      end_f_segment_partial_erase_x);

  stats->partial_erase_latency = FS_PARTIAL_ERASE_FAIL;

  for (uint8_t test = sizeof(testDelayArray) / sizeof(testDelayArray[0]); \
       test != 0; test--){
    // a delay of 1024 is equivalent to 1ms
    SRAM_p_erase(seg, testDelayArray[test]);
    if (*((uint16_t*)seg) != 0xFFFF){
      free(SRAM_p_erase);
      return;
    }
    stats->partial_erase_latency = _event_timer_value;
  }
  free(SRAM_p_erase);
}
