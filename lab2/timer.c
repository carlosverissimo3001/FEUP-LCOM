#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <lcom/lab2.h>

#include <stdint.h>
#include <stdbool.h>

#include "i8254.h"
#include "timer.h"

int counter = 0;
int hook_id = TIMER0_IRQ;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* WARNING: DO NOT CHANGE the 4 LSBs (mode and BCD/binary) of the timer's control word. */
  
  if (timer < 0 || timer > 2)
  {
    printf ("Invalid timer selection\n");
    return 1;
  }
  
  if (freq > TIMER_FREQ || freq < TIMER_FREQ_MIN) 
  {
    printf ("Invalid frequency\n");
    return 1;
  }
  
  uint8_t status;
  /* READ THE TIMER CONFING BEFORE CHANGING IT */
  if (timer_get_conf(timer, &status) != 0)
  {
    printf ("Error in timer_get_conf\n");
    return 1;
  }
  
  /* HOW TO PRESERVE THE 4 LSB´s? -> Use a bitwise operation "&(and)" beetween a word that has the 4 lsb´s set to 1 and the word from which we want to preserve the bits -> Bit(3) | Bit(2) ... and so on*/

  uint16_t div = TIMER_FREQ / freq; /* Clock/Div where div is the value loaded initially in the timer*/
  uint8_t rb = (timer << 6) | TIMER_LSB_MSB | (status & TIMER_4_LSB); 
  
  if(sys_outb(TIMER_CTRL, rb)) 
  {
    printf("Error in sys_outb\n");
    return 1;   
  }
  
  uint8_t msb, lsb;
  if (util_get_MSB(div, &msb) != 0)
  {
    printf ("Error retrieving the msb\n");
    return 1;
  }
  if (util_get_LSB(div, &lsb) != 0)
  {
    printf ("Error retrieving the lsb\n");
    return 1;
  }
  
  if(sys_outb(TIMER_0 + timer, lsb) != 0) 
  {
    printf("Error in sys_outb\n");
    return 1;
  }

  if(sys_outb(TIMER_0 + timer, msb) != 0) 
  {
    printf("Error in sys_outb\n");
    return 1;
  }
  
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;
    
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK)
  {
    printf ("sys_irqsetpolicy failed\n", hook_id);
    return 1;
  }
  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != OK)
  {
    printf("sys_irqrmpolicy failed\n");
    return 1;
  }
  return 0; 
}

void (timer_int_handler)() {
  counter++;
  return;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st){
  
  if (timer < 0 || timer > 2)
  {
    printf("Invalid timer selection\n");
    return 1;
  }
    
  uint8_t readBack = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
 
  if (sys_outb(TIMER_CTRL, readBack) != OK) 
  {
      printf ("Error in sys_outb()\n");
      return 1;
  }
  
  if (timer == 0) // 0x40
  {
    if (util_sys_inb(TIMER_0, st) != 0)
    {  
      printf ("Error reading from timer\n", timer);
      return 1;
    }
  }
  
  else if (timer == 1) // 0x41
  {
    if (util_sys_inb(TIMER_1, st) != 0)
    {  
      printf ("Error reading from timer\n", timer);
      return 1;
    }
  }
  
  else if (timer == 2) // 0x42
  {
    if (util_sys_inb(TIMER_2, st) != 0)
    {  
      printf ("Error reading from timer\n", timer);
      return 1;
    }
  }
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  
  union timer_status_field_val conf;
  //memset(&conf, 0, sizeof(union timer_status_field_val));

  if (field == tsf_all)
    conf.byte = st;
  
  else if (field == tsf_base){
    conf.bcd = (st & TIMER_BCD);
  }
  
  else if(field == tsf_initial){
    uint8_t initmode = (st & TIMER_INI_MODE) >> 4;
    if (initmode == 0x01)
      conf.in_mode = LSB_only;
    else if (initmode == 0x02)
      conf.in_mode = MSB_only;
    else if (initmode == 0x03)
      conf.in_mode = MSB_after_LSB;
    else 
      conf.in_mode = INVAL_val;
  }
  
  else if (field == tsf_mode){
    conf.count_mode = (st & TIMER_COUNT_MODE) >> 1;
    if (conf.count_mode == 0x07){
      conf.count_mode = 0x03;
    }
    if (conf.count_mode == 0x06){
      conf.count_mode = 0x02;
    }
  }
  if(timer_print_config(timer, field, conf) != 0) 
  {
    printf("Error in timer_print_config\n");
    return 1;
  }
  return 0;
}

