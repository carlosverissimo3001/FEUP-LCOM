#ifndef _LCOM_KBC_INT_H_
#define _LCOM_KBC_INT_H_

#include <stdint.h>

int kbc_error;

int(kbc_subscribe_int)(uint8_t bit_no, int *hook_id);

void(kbc_ih)(void);

int(kbc_unsubscribe_int)(int *hook_id);

#endif /* _LCOM_KBC_INT_H_ */
