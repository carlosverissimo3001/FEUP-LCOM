#include "kbc_verification.h"

#include "i8042.h"
#include "kbc_sc.h"

int(is_break_code)(void) { return !(sc_code[sc_size - 1] & BREAK_CODE_MASK); }

int(communication_error)(uint8_t st) { return st & (KBC_PAR_ERR | KBC_TO_ERR); }

int(out_buffer_full)(uint8_t st) { return st & KBC_STATUS_OBFULL; }

int(in_buffer_full)(uint8_t st) { return st & KBC_STATUS_IBFULL; }

int(aux_cleared)(uint8_t st) { return !(st & KBC_STATUS_AUX); }
