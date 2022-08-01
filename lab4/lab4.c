// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "utils.h"
#include "I8042.h"


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}/* condition */

int subscribe_kbc(uint8_t bitn, int *hook_id){
  *hook_id = bitn;
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_EXCLUSIVE|IRQ_REENABLE,hook_id);
  
}

int unsubscribe_kbc(int *hook_id){
  return sys_irqrmpolicy(hook_id);
}

int get_status(uint8_t *st){
  return util_sys_inb(STAT_REG, st);
}

int kbc_readob(uint8_t *data){
  uint8_t st;
  for (size_t i = 0; i < 10; i++)
  {
    if (get_status(&st))
    {
      return !OK;
    }
    if (st & OBF)
    {
      if(util_sys_inb(OUT_BUF, data)) return !OK;
      if (st & (TIMEOUT | PARITY)) return !OK;
      return OK;
    }
    delay(DELAY_US);
  }
  return !OK;
}

uint8_t bar[3];
uint8_t b = 0;
bool done = FALSE;
bool error = FALSE;
void (mouse_ih)(void){
  uint8_t data;
  if(kbc_readob(&data)){
    error = TRUE;
    return;
  }
  if (!(data & BIT(3)) && b == 0)
  {
    error = TRUE;
    return;
  }
  bar[b] = data;
  if(b == 2) done = TRUE;
  b = (b + 1) % 3;
}

int16_t extend(bool sb, uint8_t db){
  int16_t bs = -sb;
  return ( bs<<8 | db);
}
int make_packet(struct packet* pck){
  pck->bytes[0]= bar[0];
  pck->bytes[1]= bar[1];
  pck->bytes[2]= bar[2];
  pck->lb = (bar[0]&MOUSE_LB);
  pck->rb = (bar[0]&MOUSE_RB);
  pck->mb = (bar[0]&MOUSE_MB);
  pck->x_ov = (bar[0]&X_OVFL);
  pck->y_ov = (bar[0]&Y_OVFL);
  pck->delta_x = extend(bar[0]&X_SIGN, bar[1]);
  pck->delta_y = extend(bar[0]&Y_SIGN, bar[2]);
  return OK;
}

int send_obj(uint8_t obj, bool arg){
  int port;
  if (arg)
  {
    port = IN_BUF;
  }
  else
  {
    port = COM_PORT;
  }
  uint8_t st;
  for (size_t i = 0; i < 10; i++)
  {
    if (get_status(&st))
    {
      return !OK;
    }
    if (!(st & IBF))
    {
      if(sys_outb(port, obj)) return !OK;
      return OK;
    }
    delay(DELAY_US);
  }
  return !OK;
  
}

int make_arg(uint8_t arg){
  uint8_t data;
  for (size_t i = 0; i < 10; i++)
  {
    if(send_obj(WRT_BYTE_MOUSE, FALSE))return !OK;
    if(send_obj(arg, TRUE))return !OK;
    if (kbc_readob(&data))return !OK;
    if(data == MOUSE_NACK) delay(DELAY_US);
    if(data == MOUSE_ACK) return OK;
    if(data == MOUSE_ERROR) return !OK;
  } 
  return !OK;
}

int mouse_data_reporting(bool enable){
  if (enable)
  {
    if(make_arg(MOUSE_ENABLE_DATA_REPORTING)) return !OK;
  }
  else
  {
     if(make_arg(MOUSE_DISABLE_DATA_REPORTING)) return !OK;
  }
  return OK;
  
}

int (mouse_test_packet)(uint32_t cnt) {
  /* To be completed */
  uint8_t irq_bit = MOUSE_IRQ; 
  int hook_id;
  int r, ipc_status;
  message msg;

  mouse_data_reporting(TRUE);
  if (subscribe_kbc(irq_bit,&hook_id))
  {
    return !OK;
  }
  
  uint32_t irq_kbc_set = BIT(irq_bit);
  int escape = false;

  while (!escape) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_kbc_set) { /* Interrupt from the keyboard */
            mouse_ih();
            if (error) {
              printf("Interrupt handler failed\n");
              continue;
            }
            if (done) {
              done = FALSE;
              struct packet pkt;
              make_packet(&pkt);
              mouse_print_packet(&pkt);
              --cnt;
              if (cnt == 0)
              {
                escape= TRUE;
              }

              
            }
          }
          break;
        default:
          break; /* no onotifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }

  if (unsubscribe_kbc(&hook_id))
  {
    return !OK;
  }// Try to unsubscribe the interrupt
  if(mouse_data_reporting(FALSE)) return !OK;
  return OK;
}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s(%u): under construction\n", __func__, idle_time);
    return 1;
}

/*
int (mouse_test_gesture)() {
    
    printf("%s: under construction\n", __func__);
    return 1;
}
*/

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* To be completed */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
