#include "utils.h"

#include <lcom/lcf.h>
#include <stdint.h>

#ifdef LAB3
uint32_t counter_sys_inb = 0;
#endif

int(util_sys_inb)(int port, uint8_t *value) {
  // Avoid dereferencing  a null pointer
  if (value == NULL) return !OK; // Failure

  // Create 4 byte variable to pass as argument
  u32_t v = 0;

  // Try to call the system function
  if (sys_inb(port, &v)) return !OK; // Read Failure

#ifdef LAB3
  ++counter_sys_inb;
#endif

  // Store return value, truncation occurs automatically
  *value = v;

  return OK; // Success
}

void(delay)(int delay) { tickdelay(micros_to_ticks(delay)); }
