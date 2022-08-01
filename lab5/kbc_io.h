#ifndef _LCOM_KBC_IO_H_
#define _LCOM_KBC_IO_H_

#include <stdbool.h>
#include <stdint.h>

int tries;

int(kbc_get_status)(uint8_t *st);

int(kbc_read_outbuf)(uint8_t *buf);

int(kbc_read_return_value)(uint8_t *value);

int(kbc_issue_cmd)(uint8_t cmd, uint8_t port);

int(kbc_get_conf)(uint8_t *conf);

int(kbc_set_conf)(uint8_t conf);

#endif /* _LCOM_KBC_IO_H_ */
