#include "kbc_io.h"
#include "i8042.h"
#include "kbc_verification.h"
#include "utils.h"

int(kbc_get_status)(uint8_t *st) {
  if (st == NULL) return !OK; // Avoid dereferencing a null pointer

  //Try to get the status
  return util_sys_inb(KBC_STATUS_REG, st);
}

int(kbc_read_outbuf)(uint8_t *buf) {
  if (buf == NULL) return !OK; // Avoid dereferencing a null pointer

  //Try to read the output buffer
  return util_sys_inb(KBC_OUT_BUF, buf);
}

int(kbc_read_return_value)(uint8_t *value) {
  uint8_t status;
  tries = 0;
  do {
    ++tries;
    if (tries > KBC_MAX_TRIES) return !OK;
    if (kbc_get_status(&status)) continue; // Failed reading the status: try again
    delay(KBC_MAX_DELAY);                  // Wait for some time

    // If the output buffer is full and aux is set, we can read
  } while (!(out_buffer_full(status) && aux_cleared(status)));

  uint8_t buffer;

  //Try to read the output buffer
  if (kbc_read_outbuf(&buffer)) return !OK; //Failure reading the output buffer

  if (communication_error(status)) return !OK; //Failure communicating

  *value = buffer; //Success: write result

  return OK; //Success
}

int(kbc_issue_cmd)(uint8_t cmd, uint8_t port) {
  tries = 0;
  uint8_t status;
  do {
    ++tries;
    if (kbc_get_status(&status)) continue;                   // Failed reading the status: try again
    delay(KBC_MAX_DELAY);                                    // Wait for some time
  } while (in_buffer_full(status) && tries < KBC_MAX_TRIES); // If the in buffer is full, try again

  return sys_outb(port, cmd); // Try to issue the command
}

int(kbc_get_conf)(uint8_t *conf) {
  //Try to issue the read command
  if (kbc_issue_cmd(KBC_READ_CMD, KBC_CMD_REG)) return !OK; //Failure issuing the command

  //Try to read the output buffer
  if (kbc_read_outbuf(conf)) return !OK; //Failure reading the output buffer

  return OK; //Success
}

int(kbc_set_conf)(uint8_t conf) {
  //Try to issue the write command
  if (kbc_issue_cmd(KBC_WRITE_CMD, KBC_CMD_REG)) return !OK; //Failure issuing the command

  //Try to write the configuration
  if (kbc_issue_cmd(conf, KBC_IN_BUF)) return !OK;

  return OK; //Success
}
