#include "timer_int.h"

#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>

#include "i8254.h"

int timer_hook_id = TIMER0_IRQ;
uint32_t n_interrupts = 0; // Interrupt counter

int(timer_subscribe_int)(uint8_t *bit_no) {
  
  *bit_no = timer_hook_id;

  // Try to set the policy
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id);
}

int(timer_unsubscribe_int)() {
  // Try to remove the policy
  return sys_irqrmpolicy(&timer_hook_id);
}

void(timer_int_handler)() {
  // Increment interrupt counter
  n_interrupts++;
  return;
}
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
