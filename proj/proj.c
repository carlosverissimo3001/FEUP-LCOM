// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdbool.h>
#include <stdint.h>

// Any header files included below this line should have been created by you
#include "i8042.h"
#include "covid_escape.h"
#include "i8254.h"
#include "kbc_int.h"
#include "kbc_io.h"
#include "kbc_sc.h"
#include "kbc_verification.h"
#include "timer_int.h"
#include "vcard.h"


uint32_t irq_timer_set, irq_kbc_set; //irq_mouse_set; 

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

static int print_usage() {
  printf("Usage: <mode - hex>\n");

  return 1;
}

int(proj_main_loop)(int argc, char *argv[]) {
  uint8_t irq__kbc_bit, irq_timer_bit; //irq_mouse_bit;
  
  // Subscribe various interrupts and set video mode to indexed

  /* video mode / graphics card */
  if(vg_init(COLOR_MODE_INDEXED) == NULL) /*change to video mode 0x105 */
  {
    printf("Error initiating video card in graphics mode 0x105(Indexed Color Mode)(vg_init)\n");
    return 1;
  }
  
  /* timer */
  if (timer_subscribe_int(&irq_timer_bit) != 0){
    printf("Error while subscribing timer interrupts\n");
    return 1;
  }

  /* keyboard */
  if (kbc_subscribe_int(&irq__kbc_bit) != 0){
    printf("Error while subscribing keyboard interrupts\n");
    return 1;
  }
  
  /* mouse 
  if (mouse_subscribe_int(&irq_mouse_bit) != 0){
    printf("Error while subscribing mouse interrupts\n");
    return 1;
  }
  
  if (mouse_data_reporting(TRUE) != 0)  Enables data reporting 
  {
    printf("Error while enabling mouse data reporting\n");
    return 1;
  }
  */
  irq_timer_set = BIT(irq_timer_bit);
  irq_kbc_set = BIT(irq__kbc_bit);
  //irq_mouse_set = BIT(irq_mouse_bit);
  //printf ("Linha 93\n");
  create_xpms();
  //printf ("Linha 95\n");
  play_game();

  /* Unsubscribe from the interrupts */

  
  /* mouse related 
  if (mouse_data_reporting(FALSE) != 0)  Disables data reporting 
  {
    printf("Error while disabling mouse data reporting\n");
    return 1;
  }

  if (mouse_unsubscribe_int() != 0){
    printf("Error while unsubscribing mouse interrupts\n");
    return 1;
  }*/
  
  /* keyboard related */
  if (kbc_unsubscribe_int() != 0){
    printf("Error while unsubscribing keyboard interrupts\n");
    return 1;
  }
  
  /* timer */
  if (timer_unsubscribe_int() != 0){
    printf("Error while unsubscribing timer interrupts\n");
    return 1;
  }
  
  /* video mode / graphics card */
 if (vg_exit() != 0){
    printf("Error resetting the video controller to operate in text mode\n");
    return 1;
  }
  
  bool const minix3_logo = true;
  bool const grayscale = false;
  uint8_t const delay = 5;
  uint16_t mode;

  if (argc != 1)
    return print_usage();

  // parse mode info (it has some limitations for the sake of simplicity)
  if (sscanf(argv[0], "%hx", &mode) != 1) {
    printf("%s: invalid mode (%s)\n", __func__, argv[0]);

    return print_usage();
  }

  return proj_demo(mode, minix3_logo, grayscale, delay);
}
