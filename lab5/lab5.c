// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include "math.h"

// Any header files included below this line should have been created by you

#include "vcard.h"
#include "sprite.h"
#include "kbc_int.h"
#include "kbc_sc.h"
#include "kbc_verification.h"
#include "timer_int.h"
#include "i8042.h"
#include "i8254.h"



extern uint32_t n_interrupts;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  //Uses the VBE INT 0x10 interface to set the desired graphics mode. After delay seconds, it should go back to Minix's default text mode.
  
  if (vg_init(mode) == NULL)
  {
    printf("Error while initiating video card in graphics mode (vg_init)\n");
    return 1;
  }
  
  sleep(delay);
  
  /* Reset back to text mode  */
  if (vg_exit() != 0){
    printf("Error while resetting the video controller to operate in text mode\n");
    return 1;
  }
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
                  
  if(vg_init(mode) == NULL)
  {
      printf("Error while initiating video card in graphics mode (vg_init)\n");
      return 1;
  }
  
  if (vg_draw_rectangle(x, y, width, height, color) != 0){
    printf ("Error drawing the rectangle\n");
    if (vg_exit() != 0){
      printf("Error resetting the video controller to operate in text mode\n");
      return 1;
    }
    return 1;
  }
  
  /* Returns upon release of the ESC key -> Use keyboard interruption */  
  uint8_t irq_bit;

  // Try to subscribe the interrupt
  if (kbc_subscribe_int(&irq_bit))
    return 1;

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
              //kbd_print_scancode(is_break_code(), sc_size, sc_code);
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
  if (kbc_unsubscribe_int()) {
      printf("Error unsubscribing kbc interrupts\n");
      if (vg_exit() != 0){
        printf("Error resetting the video controller to operate in text mode\n");
        return 1;
      }
      return 1;
  }
  
  if (vg_exit() != 0){
      printf("Error resetting the video controller to operate in text mode\n");
      return 1;
    }
  
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if(vg_init(mode) == NULL)
  {
      printf("Error initiating video card in graphics mode (vg_init)\n");
      return 1;
  }

  if (vg_draw_rectangle_pattern(mode, no_rectangles, first, step) != 0)
  {
    printf("Error while drawing the pattern\n");
    if (vg_exit() != 0){
      printf("Error resetting the video controller to operate in text mode\n");
      return 1;
    }
    return 1;
  }
  
  /* Returns upon release of the ESC key -> Use keyboard interruption */  
  uint8_t irq_bit;

  // Try to subscribe the interrupt
  if (kbc_subscribe_int(&irq_bit))
    return 1;

  int r, ipc_status;
  message msg;

  uint32_t irq_kbc_set = BIT(irq_bit);
  int terminate = 0;

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
              //kbd_print_scancode(is_break_code(), sc_size, sc_code);
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
  if (kbc_unsubscribe_int()) {
      printf("Error unsubscribing kbc interrupts\n");
      if (vg_exit() != 0){
        printf("Error resetting the video controller to operate in text mode\n");
        return 1;
      }
      return 1;
  }
  
  if (vg_exit() != 0){
    printf("Error resetting the video controller to operate in text mode\n");
    return 1;
  }
  
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  
  if(vg_init(COLOR_MODE_INDEXED) == NULL) /*change to video mode 0x105 */
  {
      printf("Error initiating video card in graphics mode 0x105(Indexed Color Mode)(vg_init)\n");
      return 1;
  }
  
  /*from slides*/
  
  /* xpm_map_t xmap; -> Already given by the functions arguments */
  xpm_image_t img;
  uint8_t *map; 
  // get the pixmap from the XPM
  map = xpm_load(xpm, XPM_INDEXED, &img);
  // error checking
  if (map == NULL){
    printf("Error loading xpm\n");
    if (vg_exit() != 0){
      printf("Error resetting the video controller to operate in text mode\n");
      return 1;
    }
    return 1;
  }
  
  /* 
    copy it to graphics memory 
  */
  if (pixmap_draw(img, map ,x, y) != 0)
  {
    printf("Error drawing pixmap\n");
    if (vg_exit() != 0){
      printf("Error resetting the video controller to operate in text mode\n");
      return 1;
    }
    return 1;
  }

  /* Returns upon release of the ESC key -> Use keyboard interruption */  
  uint8_t irq_bit;

  // Try to subscribe the interrupt
  if (kbc_subscribe_int(&irq_bit))
    return 1;

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
              //kbd_print_scancode(is_break_code(), sc_size, sc_code);
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
  
  if (kbc_unsubscribe_int()) {
      printf("Error unsubscribing kbc interrupts\n");
      return 1;
  }
  
  if (vg_exit() != 0){
    printf("Error resetting the video controller to operate in text mode\n");
    return 1;
  }
  
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  
  if (speed == 0)
  {
    printf("Speed has to be a number different form zero\n");
    return 1;
  }
    
  
  if(vg_init(COLOR_MODE_INDEXED) == NULL) /*change to video mode 0x105 */
  {
      printf("Error initiating video card in graphics mode 0x105(Indexed Color Mode)(vg_init)\n");
      return 1;
  }
  /* NEED TO subscribe interrupts from both keyboard and timer(because of "uint8_t fr_rate") */
  
  uint8_t timer_hook_id;
  uint8_t irq_kbc_bit;
  
  /* Subscribe timer interruptions */
  if(timer_subscribe_int(&timer_hook_id) != 0){ 
    printf("Error subscribing the timer interrupts\n");
    if (vg_exit() != 0){
      printf("Error resetting the video controller to operate in text mode\n");
      return 1;
    }
    return 1;
  }
  
  /* Subscribe keyboard interruptions */
  if (kbc_subscribe_int(&irq_kbc_bit)) {
    printf("Error subscribing the kbc interrupts\n");
    if (vg_exit() != 0){
      printf("Error resetting the video controller to operate in text mode\n");
      return 1;
    }
    return 1;
  }
  /*
  IMP: You need only consider movements along either the horizontal or the vertical directions. I.e. either xf is equal to xi or yf is equal to yi.

  IMP: If speed is positive it should be understood as the displacement in pixels between consecutive frames. If the speed is negative it should be understood as the number of frames required for a displacement of one pixel.

  */
  Sprite *sp;
  int mov_Len;
  
  /* 
  Check if the movement is along the x or the y axis, i.e,  
  -> if the initial x position = final x position the movement is vertical 
  -> if the initial y position = final y position the movement is horizontal 
  */
  
  if(xi == xf){ /* movement along the y ayis -> vertical movement */
    /* calculate the length of the movement */
    mov_Len = abs(yi - yf);
    if (yi > yf){
      
      if (speed > 0){ /* it should be understood as the displacement in pixels between consecutive frames */
        sp = create_sprite(xpm, xi, yi, 0, -speed); /* argument speed has to have a 'minus' since y decrements */
      }
      else if (speed < 0){ /* it should be understood as the number of frames required for a displacement of one pixel. */

        sp = create_sprite(xpm, xi, yi, 0, -1); /* same here */
      }
    
    }
    else if (yf > yi){
      if (speed > 0){ /* it should be understood as the displacement in pixels between consecutive frames */
        sp = create_sprite(xpm, xi, yi, 0, speed);
      }
      else if (speed < 0){ /* it should be understood as the number of frames required for a displacement of one pixel. */
        sp = create_sprite(xpm, xi, yi, 0, 1);
      }
    }
    else /* Movement is not vertical nor horizontal */
    {
      printf("Movement is not along the x nor the y axis\n");
      if (vg_exit() != 0){
        printf("Error resetting the video controller to operate in text mode\n");
        return 1;
      }
      return 1;
    }
    
  }
  
  else if(yi == yf){ /* movement along the x ayis -> horizontal movement */
    mov_Len = abs(xi - xf);
    if (xi > xf){
      if (speed > 0){ /* it should be understood as the displacement in pixels between consecutive frames */
        sp = create_sprite(xpm, xi, yi,-speed, 0); /* argument speed has to have a 'minus' since x decrements */
      }
      else if (speed < 0){ /* it should be understood as the number of frames required for a displacement of one pixel. */

        sp = create_sprite(xpm, xi, yi, -1, 0); /* same here */
      }
    }
    else if (xf > xi){
      if (speed > 0){ /* it should be understood as the displacement in pixels between consecutive frames */
        sp = create_sprite(xpm, xi, yi, speed, 0);
      }
      else if (speed < 0){ /* it should be understood as the number of frames required for a displacement of one pixel. */
        sp = create_sprite(xpm, xi, yi, 1, 0);
      }
    }
    else /* Movement is not vertical nor horizontal */
    {
      printf("Movement is not along the x nor the y axis\n");
      if (vg_exit() != 0){
        printf("Error resetting the video controller to operate in text mode\n");
        return 1;
      }
      return 1;
    }
  
   
  }
  else /* Movement is not vertical nor horizontal */
  {
    printf("Movement is not along the x nor the y axis\n");
    if (vg_exit() != 0){
      printf("Error resetting the video controller to operate in text mode\n");
      return 1;
    }
    return 1;
  }
  
  /*When the user releases the ESC key (scancode 0x81), it should reset the video mode to Minix's default text mode and return, even if the movement has NOT been completed. */
  
  /*
  IMP: You must always ensure that the final position of the pixmap is the one specified in the arguments to video_test_move(). Thus if speed is positive and the length of the movement is not a multiple of speed, the last displacement of the pixmap in its movement will be smaller than speed.
  */

  
  xpm_image_t img_move;
  uint8_t *map_move; 
  // get the pixmap from the XPM
  map_move = xpm_load(xpm, XPM_INDEXED, &img_move);

  if (pixmap_draw(img_move, map_move, xi, yi) != 0)
  {
    printf("Error drawing pixmap\n");
    if (vg_exit() != 0){
      printf("Error resetting the video controller to operate in text mode\n");
      return 1;
    }
    return 1;
  }
  
  int r, ipc_status, terminate = false;
  message msg;

  uint32_t irq_kbc_set = BIT(irq_kbc_bit);
  uint32_t irq_timer_set = BIT(timer_hook_id);

  int elapsed_frames = 0;

  
  uint8_t fps = sys_hz() / fr_rate;
  //timer_set_frequency(TIMER_0, fps);


  while (!terminate) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status))) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_kbc_set){ /* Interrupt from the keyboard */
            kbc_ih();
            if (kbc_error) {
              printf("Failed in interrupt handler\n");
              continue;
            }
            if (kbc_done) {
              //kbd_print_scancode(is_break_code(), sc_size, sc_code);
              sc_size = 0;
            }
            if (sc_code[0] == ESC_BRK_CODE) terminate = 1;
          }
          if (msg.m_notify.interrupts & irq_timer_set) { /* Interrupt from the timer */
            timer_int_handler();                         /* Handle the interrupt */
            //vg_draw_rectangle(0, 0, 50, 50, 7);
            if (speed > 0){
              if((n_interrupts % fps == 0) && (mov_Len > 0)){
                /* deletes the region where the pixmap was drawn*/
                erase_sprite(sp);
                
                move_sprite(sp);
                
                /* update length beetween xf and xi or yf and yi */
                mov_Len = mov_Len - speed;

                if (mov_Len < 0){
                    if (sp->xspeed == 0)
                      sp->yspeed = speed - abs(mov_Len);
                    if (sp->yspeed == 0) 
                      sp->xspeed = speed - abs(mov_Len);
                    move_sprite(sp); 
                }
                draw_sprite(sp, img_move);
              }
            }
            else /* negative speed -> it should be understood as the number of frames required for a displacement of one pixel. */
            {
              if ((n_interrupts % fps == 0))
                elapsed_frames++;
              if ((elapsed_frames == -speed) && (mov_Len>0)){
                /* clear the region where the sprite was drawn*/
                erase_sprite(sp);
                
                move_sprite(sp);

                /* update length beetween xf and xi or yf and yi */
                mov_Len = mov_Len - 1;

                elapsed_frames = 0;

                draw_sprite(sp, img_move);
              }
            }
          } 
          /*You must always ensure that the final position of the pixmap is the one specified in the arguments to video_test_move(). Thus if speed is positive and the length of the movement is not a multiple of speed, the last displacement of the pixmap in its movement will be smaller than speed.*/
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  
  }
  destroy_sprite(sp);
  
  if (kbc_unsubscribe_int()) {
    printf("Error unsubscribing from the kbc interrupts\n");
    if (vg_exit() != 0){
      printf("Error resetting the video controller to operate in text mode\n");
      return 1;
    }
    return 1;
  }

  if (timer_unsubscribe_int()) {
    printf("Error unsubscribing from the timer interrupts\n");
    if (vg_exit() != 0){
      printf("Error resetting the video controller to operate in text mode\n");
      return 1;
    }
    return 1;
  }

  if (vg_exit() != 0){
    printf("Error resetting the video controller to operate in text mode\n");
    return 1;
  }
  
  return 0;
}

int(video_test_controller)() {
  return 1;
}
