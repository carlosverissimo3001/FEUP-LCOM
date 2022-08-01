#include "timer_int.h"

#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>

#include "i8254.h"

int hook_id = TIMER0_IRQ;
uint32_t n_interrupts = 0; // Interrupt counter

int(timer_subscribe_int)(uint8_t *bit_no) {
  // Avoid dereferencing a null pointer
  if (bit_no == NULL) return !OK;

  *bit_no = hook_id;

  // Try to set the policy
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id);
}

int(timer_unsubscribe_int)() {
  // Try to remove the policy
  return sys_irqrmpolicy(&hook_id);
}

void(timer_int_handler)() {
  // Increment interrupt counter
  ++n_interrupts;
  return;
}
