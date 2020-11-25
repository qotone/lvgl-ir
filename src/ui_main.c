#include "lvgl/lvgl.h"

#include "lv_drivers/display/fbdev.h"
//#include "lv_drivers/indev/mouse_hid.h"
//#include "lv_examples/lv_apps/demo/demo.h"
#include "lvgl.h"
#include "lv_ex_conf.h"


#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "keypad.h"

static pthread_t tid;
static int lv_running = 0, lv_w = 1280, lv_h = 1024;

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_group_t*  g;
static lv_obj_t * tv;
static lv_obj_t * t1;
static lv_obj_t * t2;
static lv_obj_t * t3;



static void* lvgl_main(void* p);


static void selectors_create(lv_obj_t * parent);
static void text_input_create(lv_obj_t * parent);
static void msgbox_create(void);

static void focus_cb(lv_group_t * g);
static void msgbox_event_cb(lv_obj_t * msgbox, lv_event_t e);
static void tv_event_cb(lv_obj_t * ta, lv_event_t e);
static void ta_event_cb(lv_obj_t * ta, lv_event_t e);
static void kb_event_cb(lv_obj_t * kb, lv_event_t e);


struct {
    lv_obj_t * btn;
    lv_obj_t * cb;
    lv_obj_t * slider;
    lv_obj_t * sw;
    lv_obj_t * spinbox;
    lv_obj_t * dropdown;
    lv_obj_t * roller;
    lv_obj_t * list;
}selector_objs;

struct {
    lv_obj_t * ta1;
    lv_obj_t * ta2;
    lv_obj_t * kb;
}textinput_objs;



/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_EVENT_CB_DECLARE(dd_enc)
{
    if(e == LV_EVENT_VALUE_CHANGED) {
        /*printf("chg\n");*/
    }
}

int lvgl_stop(void)
{
  lv_running = 0;
  keypad_uninit();
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

#if 0
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
    g = lv_group_create();
    lv_group_set_focus_cb(g, focus_cb);
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
    lv_indev_set_group(indev_keypad, g);
    /* Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     * add objects to the group with `lv_group_add_obj(group, obj)`
     * and assign this input device to group to navigate in it:
     * `lv_indev_set_group(indev_keypad, group);` */

#endif

    /*Create a Demo*/
    //lv_obj_t* tv = demo_create();
    tv = lv_tabview_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(tv, tv_event_cb);

    t1 = lv_tabview_add_tab(tv, "Selectors");
    t2 = lv_tabview_add_tab(tv, "Text input");

    lv_group_add_obj(g, tv);

    selectors_create(t1);
    text_input_create(t2);

    msgbox_create();

    /*Handle LitlevGL tasks (tickless mode)*/
    while(lv_running) {

        lv_task_handler();
        usleep(5*1000);

// no use
#if 0
        // test;
        static cnt = 0;
        if(++cnt%200 == 0)
{
                static uint8_t tab = 0;
                tab++;
                if(tab >= 3) tab = 0;
                lv_tabview_set_tab_act(tv, tab, true);
            }
#endif
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




/**********************
 *   STATIC FUNCTIONS
 **********************/

static void selectors_create(lv_obj_t * parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_COLUMN_MID);

    selector_objs.btn = lv_btn_create(parent, NULL);

    lv_obj_t * label = lv_label_create(selector_objs.btn, NULL);
    lv_label_set_text(label, "Button");

    selector_objs.cb = lv_checkbox_create(parent, NULL);

    selector_objs.slider = lv_slider_create(parent, NULL);
    lv_slider_set_range(selector_objs.slider, 0, 10);

    selector_objs.sw = lv_switch_create(parent, NULL);

    selector_objs.spinbox = lv_spinbox_create(parent, NULL);

    selector_objs.dropdown = lv_dropdown_create(parent, NULL);
    lv_obj_set_event_cb(selector_objs.dropdown, dd_enc);

    selector_objs.roller = lv_roller_create(parent, NULL);

    selector_objs.list = lv_list_create(parent, NULL);
    if(lv_obj_get_height(selector_objs.list) > lv_page_get_height_fit(parent)) {
        lv_obj_set_height(selector_objs.list, lv_page_get_height_fit(parent));
    }
    lv_list_add_btn(selector_objs.list, LV_SYMBOL_OK, "Apply");
    lv_list_add_btn(selector_objs.list, LV_SYMBOL_CLOSE, "Close");
    lv_list_add_btn(selector_objs.list, LV_SYMBOL_EYE_OPEN, "Show");
    lv_list_add_btn(selector_objs.list, LV_SYMBOL_EYE_CLOSE, "Hide");
    lv_list_add_btn(selector_objs.list, LV_SYMBOL_TRASH, "Delete");
    lv_list_add_btn(selector_objs.list, LV_SYMBOL_COPY, "Copy");
    lv_list_add_btn(selector_objs.list, LV_SYMBOL_PASTE, "Paste");
}

static void text_input_create(lv_obj_t * parent)
{
    textinput_objs.ta1 = lv_textarea_create(parent, NULL);
    lv_obj_set_event_cb(textinput_objs.ta1, ta_event_cb);
    lv_obj_align(textinput_objs.ta1, NULL, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 20);
    lv_textarea_set_one_line(textinput_objs.ta1, true);
    lv_textarea_set_cursor_hidden(textinput_objs.ta1, true);
    lv_textarea_set_placeholder_text(textinput_objs.ta1, "Type something");
    lv_textarea_set_text(textinput_objs.ta1, "");

    textinput_objs.ta2 = lv_textarea_create(parent, textinput_objs.ta1);
    lv_obj_align(textinput_objs.ta2, textinput_objs.ta1, LV_ALIGN_OUT_BOTTOM_MID, 0, LV_DPI / 20);

    textinput_objs.kb = NULL;
}

static void msgbox_create(void)
{
    lv_obj_t * mbox = lv_msgbox_create(lv_layer_top(), NULL);
    lv_msgbox_set_text(mbox, "Welcome to the keyboard and encoder demo");
    lv_obj_set_event_cb(mbox, msgbox_event_cb);
    lv_group_add_obj(g, mbox);
    lv_group_focus_obj(mbox);

    lv_group_focus_freeze(g, true);

    static const char * btns[] = {"Ok", "Cancel", ""};
    lv_msgbox_add_btns(mbox, btns);
    lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_local_bg_opa(lv_layer_top(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_70);
    lv_obj_set_style_local_bg_color(lv_layer_top(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_click(lv_layer_top(), true);
}


static void msgbox_event_cb(lv_obj_t * msgbox, lv_event_t e)
{
    if(e == LV_EVENT_CLICKED) {
        uint16_t b = lv_msgbox_get_active_btn(msgbox);
        if(b == 0 || b == 1) {
            lv_obj_del(msgbox);
            lv_obj_reset_style_list(lv_layer_top(), LV_OBJ_PART_MAIN);
            lv_obj_set_click(lv_layer_top(), false);
            lv_event_send(tv, LV_EVENT_REFRESH, NULL);
        }
    }
}

static void focus_cb(lv_group_t * group)
{
    lv_obj_t * obj = lv_group_get_focused(group);
    if(obj != tv) {
        uint16_t tab = lv_tabview_get_tab_act(tv);
        switch(tab) {
        case 0:
            lv_page_focus(t1, obj, LV_ANIM_ON);
            break;
        case 1:
            lv_page_focus(t2, obj, LV_ANIM_ON);
            break;
        case 2:
            lv_page_focus(t3, obj, LV_ANIM_ON);
            break;
        }
    }
}

static void tv_event_cb(lv_obj_t * ta, lv_event_t e)
{
    if(e == LV_EVENT_VALUE_CHANGED || e == LV_EVENT_REFRESH) {
        lv_group_remove_all_objs(g);

        uint16_t tab = lv_tabview_get_tab_act(tv);
        size_t size = 0;
        lv_obj_t ** objs = NULL;
        if(tab == 0) {
            size = sizeof(selector_objs);
            objs = (lv_obj_t**) &selector_objs;
        }
        else if(tab == 1) {
            size = sizeof(textinput_objs);
            objs = (lv_obj_t**) &textinput_objs;
        }

        lv_group_add_obj(g, tv);

        uint32_t i;
        for(i = 0; i < size / sizeof(lv_obj_t *); i++) {
            if(objs[i] == NULL) continue;
            lv_group_add_obj(g, objs[i]);
        }

    }

}

static void ta_event_cb(lv_obj_t * ta, lv_event_t e)
{
    /*Create a virtual keyboard for the encoders*/
    lv_indev_t * indev = lv_indev_get_act();
    if(indev == NULL) return;
    lv_indev_type_t indev_type = lv_indev_get_type(indev);

    if(e == LV_EVENT_FOCUSED) {
        lv_textarea_set_cursor_hidden(ta, false);
        if(lv_group_get_editing(g)) {
            if(textinput_objs.kb == NULL) {
                textinput_objs.kb = lv_keyboard_create(lv_scr_act(), NULL);
                lv_group_add_obj(g, textinput_objs.kb);
                lv_obj_set_event_cb(textinput_objs.kb, kb_event_cb);
                lv_obj_set_height(tv, LV_VER_RES - lv_obj_get_height(textinput_objs.kb));
            }

            lv_keyboard_set_textarea(textinput_objs.kb, ta);
            lv_group_focus_obj(textinput_objs.kb);
            lv_group_set_editing(g, true);
            lv_page_focus(t2, lv_textarea_get_label(ta), LV_ANIM_ON);
        }
    }
    else if(e == LV_EVENT_DEFOCUSED) {
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            if(textinput_objs.kb == NULL) {
                lv_textarea_set_cursor_hidden(ta, true);
            }
        } else {
            lv_textarea_set_cursor_hidden(ta, true);
        }
    }
}

static void kb_event_cb(lv_obj_t * kb, lv_event_t e)
{
    lv_keyboard_def_event_cb(kb, e);

    if(e == LV_EVENT_APPLY || e == LV_EVENT_CANCEL) {
        lv_group_focus_obj(lv_keyboard_get_textarea(kb));
        lv_obj_del(kb);
        textinput_objs.kb = NULL;
        lv_obj_set_height(tv, LV_VER_RES);
    }
}
