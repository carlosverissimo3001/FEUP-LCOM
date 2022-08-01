#ifndef _LCOM_LAB3_UTILS_H
#define _LCOM_LAB3_UTILS_H

#include <stdint.h>

#ifdef LAB3
uint32_t counter_sys_inb;
#endif

int(util_sys_inb)(int port, uint8_t *value);

void(delay)(int delay);

#endif /* _LCOM_LAB3_UTILS_H */
