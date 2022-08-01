#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "i8254.h"
#include "kbc_int.h"
#include "kbc_io.h"
#include "kbc_poll.h"
#include "kbc_sc.h"
#include "kbc_verification.h"
#include "timer_int.h"
#include "utils.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  // lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  // lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv)) return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t irq_bit = KBC_IRQ;
  int hook_id;

  // Try to subscribe the interrupt
  if (kbc_subscribe_int(irq_bit, &hook_id)) return 1;

  int r, ipc_status;
  message msg;

  uint32_t irq_kbc_set = BIT(irq_bit);
  int terminate = false;

  while (!terminate) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_kbc_set) { /* Interrupt from the keyboard */
            kbc_ih();
            if (kbc_error) {
              printf("Failed in interrupt handler\n");
              continue;
            }
            if (kbc_done) {
              kbd_print_scancode(is_break_code(), sc_size, sc_code);
              sc_size = 0;
            }
            if (sc_code[0] == ESC_BRK_CODE) terminate = 1;
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }
  // Try to unsubscribe the interrupt
  if (kbc_unsubscribe_int(&hook_id)) return !OK;

  if (kbd_print_no_sysinb(counter_sys_inb)) return !OK;

  return OK;
}

int(kbd_test_poll)() {
  uint8_t conf;
  kbc_get_conf(&conf);

  do {
    if (kbc_poll()) continue; //Polling failed

    if (kbc_done) {
      kbd_print_scancode(!(sc_code[sc_size - 1] & BREAK_CODE_MASK), sc_size, sc_code);
      sc_size = 0;
    }
  } while (sc_code[0] != ESC_BRK_CODE);

  conf |= KBC_ENABLE_INTERRUPT;
  kbc_set_conf(conf);

  if (kbd_print_no_sysinb(counter_sys_inb)) return !OK;

  return OK;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t irq_kbc_bit = KBC_IRQ;
  extern uint32_t n_interrupts;
  uint8_t timer_hook_id;
  int kbc_hook_id;

  const int frequency = 60; // Assuming the starting frequency
  int time = 0;

  // Try to subscribe the timer interrupt
  if (timer_subscribe_int(&timer_hook_id)) return 1;

  uint32_t irq_timer_set = BIT(timer_hook_id);

  // Try to subscribe the keyboard interrupt
  if (kbc_subscribe_int(irq_kbc_bit, &kbc_hook_id)) return 1;

  int r, ipc_status;
  message msg;

  uint32_t irq_kbc_set = BIT(irq_kbc_bit);
  int terminate = false;

  while (!terminate) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* Received a notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 /* Hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_kbc_set) { /* Interrupt from the keyboard */
            time = 0;                                  /* Start timing */
            n_interrupts = 0;
            kbc_ih();                /* Handle the interrupt */
            if (kbc_error) continue; //Interrupt handler failed

            if (kbc_done) {
              kbd_print_scancode(is_break_code(), sc_size, sc_code);
              sc_size = 0;
            }
            if (sc_code[0] == ESC_BRK_CODE) terminate = true;
          }

          if (msg.m_notify.interrupts & irq_timer_set) { /* Interrupt from the timer */
            timer_int_handler();                         /* Handle the interrupt */
            if (n_interrupts % frequency == 0) ++time;   /* Second has elapsed */
            if (time >= n) terminate = true;                /* Took too long */
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }

  // Try to unsubscribe the keyboard interrupt
  if (kbc_unsubscribe_int(&kbc_hook_id)) return !OK;

  // Try to unsubscribe the timer interrupt
  if (timer_unsubscribe_int()) return !OK;

  //Print number of sysinb calls
  if (kbd_print_no_sysinb(counter_sys_inb)) return !OK;

  return OK;

  return 1;
}
