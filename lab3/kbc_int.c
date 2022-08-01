#include "kbc_int.h"

#include <stdint.h>

#include "i8042.h"
#include "kbc_io.h"
#include "kbc_sc.h"
#include "kbc_verification.h"

int(kbc_subscribe_int)(uint8_t bit_no, int *hook_id) {
  if (hook_id == NULL) return !OK; // Avoid dereferencing a null pointer

  *hook_id = bit_no; //Set the hook id

  // Try to set the policy
  return sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_id);
}

void(kbc_ih)(void) {
  kbc_error = false; // Everything OK so far

  // Try to read the status
  uint8_t status;
  if (kbc_get_status(&status)) {
    kbc_error = true; // Failed to read the status
    return;
  }

  uint8_t buffer;
  if (kbc_read_outbuf(&buffer)) {
    kbc_error = true; // Failed to read the output buffer
    return;
  }
  sc_code[sc_size] = buffer; // Save the scancode
  ++sc_size;                 // Added a scancode
  kbc_done = (buffer != EXTEND_SCODE);

  if (kbc_get_status(&status)) {
    kbc_error = true; // Failed to read the status
    return;
  }

  if (communication_error(status)) {
    kbc_error = true; // Parity or timeout eror
    return;
  }
}

int(kbc_unsubscribe_int)(int *hook_id) {
  // Avoid dereferencing a null pointer
  if (hook_id == NULL) return !OK;

  // Try to remove the policy
  return sys_irqrmpolicy(hook_id);
}
