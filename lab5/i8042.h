#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KBC_IRQ 1

#define KBC_IN_BUF 0x60
#define KBC_OUT_BUF 0x60 

#define KBC_CMD_REG 0x64
#define KBC_STATUS_REG 0x64

#define KBC_ENABLE_INTERRUPT BIT(0)
#define KBC_STATUS_AUX BIT(5)

#define EXTEND_SCODE 0xE0
#define ESC_BRK_CODE 0x81
#define BREAK_CODE_MASK BIT(7)

#define KBC_STATUS_IBFULL BIT(1)
#define KBC_STATUS_OBFULL BIT(0)
#define KBC_PAR_ERR BIT(7)
#define KBC_TO_ERR BIT(6)

#define KBC_READ_CMD 0x20
#define KBC_WRITE_CMD 0x60

#define KBC_MAX_DELAY 20000
#define KBC_MAX_TRIES 20

#endif /* _LCOM_I8042_H */
