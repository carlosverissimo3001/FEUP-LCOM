#include "kbc_poll.h"

#include "i8042.h"
#include "kbc_io.h"
#include "kbc_sc.h"

int(kbc_poll)(void) {
  uint8_t buffer;
  if (kbc_read_return_value(&buffer)) return !OK;
  sc_code[sc_size] = buffer;           // Save the scancode
  ++sc_size;                           // Added a scancode
  kbc_done = (buffer != EXTEND_SCODE); // Read another if there are two
  return OK;
}
