#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/mouse_hid.h"
#include "lv_examples/lv_apps/demo/demo.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "keypad.h"

static pthread_t tid;
static int lv_running = 0, lv_w = 1280, lv_h = 1024;
static void* lvgl_main(void* p);

int lvgl_stop(void)
{
  lv_running = 0;
  return pthread_join(tid, NULL);
}

int lvgl_start(int w, int h)
{
  lv_w = w;
  lv_h = h;
  lv_running = 1;
  return pthread_create(&tid, NULL, lvgl_main, NULL);
}


static void* lvgl_main(void* p)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*Initialize and register a display driver*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    /*Set the resolution of the display*/
    disp_drv.hor_res = lv_w; //1280
    disp_drv.ver_res = lv_h; //1024
    #define DISP_BUF_SIZE (80*LV_HOR_RES_MAX/*1280*/)

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];
    /*Initialize a descriptor for the buffer*/
    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    lv_disp_drv_register(&disp_drv);

#if 1
    mouse_hid_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_hid_read;     /*This function will be called periodically (by the library) to get the mouse position and state*/
    lv_indev_t * mouse_indev = lv_indev_drv_register(&indev_drv);

    LV_IMG_DECLARE(mouse_ico);
    lv_obj_t * cursor_obj =  lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(cursor_obj, &mouse_ico);
    //lv_img_set_src(cursor_obj, LV_SYMBOL_CALL);
    lv_indev_set_cursor(mouse_indev, cursor_obj);
#else
    /*------------------
     * Keypad
     * -----------------*/

    /*Initialize your keypad or keyboard if you have*/
    keypad_init();
    lv_indev_drv_t kp_drv;
    /*Register a keypad input device*/
    lv_indev_drv_init(&kp_drv);
    kp_drv.type = LV_INDEV_TYPE_KEYPAD;
    kp_drv.read_cb = keypad_read;
    lv_indev_t * indev_keypad = lv_indev_drv_register(&kp_drv);

    /* Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     * add objects to the group with `lv_group_add_obj(group, obj)`
     * and assign this input device to group to navigate in it:
     * `lv_indev_set_group(indev_keypad, group);` */

#endif

    /*Create a Demo*/
    lv_obj_t* tv = demo_create();

    /*Handle LitlevGL tasks (tickless mode)*/
    while(lv_running) {

        lv_task_handler();
        usleep(5*1000);

        // test;
        static cnt = 0;
        if(++cnt%200 == 0)
        {
          static uint8_t tab = 0;
          tab++;
          if(tab >= 3) tab = 0;
          lv_tabview_set_tab_act(tv, tab, true);
        }
    }

    return NULL;
}


/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
