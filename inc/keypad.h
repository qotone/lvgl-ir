#ifndef __KEYPAD_H
#define __KEYPAD_H
#include "lv_drv_conf.h"
#include "lvgl.h"

/* Initialize your keypad */
void keypad_init(void);


/* Will be called by the library to read the mouse */
bool keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

void keypad_uninit(void);

#endif
