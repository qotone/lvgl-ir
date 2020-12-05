#ifndef __KEYPAD_H
#define __KEYPAD_H
#include "lv_drv_conf.h"
#include "lvgl.h"
#include "hiir.h"
#include "ir_code.h"

extern uint32_t last_key;
extern lv_indev_state_t state;

typedef void (* keypad_callback)(irkey_info_s irkey);

/* Initialize your keypad */
void keypad_init(void);


/* Will be called by the library to read the mouse */
bool keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

void keypad_uninit(void);

#endif
