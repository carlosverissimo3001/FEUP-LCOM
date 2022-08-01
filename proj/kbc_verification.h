#ifndef _LCOM_KBC_VERIFICATION_H_
#define _LCOM_KBC_VERIFICATION_H_

#include <stdbool.h>
#include <stdint.h>

int(is_break_code)();

int(communication_error)(uint8_t st);

int(communication_error)(uint8_t st);

int(out_buffer_full)(uint8_t st);

int(in_buffer_full)(uint8_t st);

int(aux_cleared)(uint8_t st);

#endif /* _LCOM_KBC_VERIFICATION_H_ */
