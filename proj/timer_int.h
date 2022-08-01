#ifndef _LCOM_KBC_INT_H_
#define _LCOM_KBC_INT_H_

#include <stdint.h>
#include <lcom/lcf.h>

int hook_id;
uint32_t n_interrupts;

int(timer_subscribe_int)(uint8_t *bit_no);

int(timer_unsubscribe_int)();

void(timer_int_handler)();

int (timer_set_frequency)(uint8_t timer, uint32_t freq);

void(delay)(int delay){ 
  tickdelay(micros_to_ticks(delay)); 
}

#endif /* _LCOM_KBC_INT_H_ */
