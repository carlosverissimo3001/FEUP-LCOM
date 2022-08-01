int (timer_set_frequency)(uint8_t timer, uint32_t freq);

int (timer_subscribe_int)(uint8_t *bit_no);

void (timer_int_handler)();

int (timer_get_conf)(uint8_t timer, uint8_t *st);

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field);
