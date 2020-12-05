#include "ui_setting_panel.h"


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>

#define PANEL_WIDTH   (800)
#define PANEL_HEIGH   (600)

lv_obj_t *tv;
lv_obj_t *t1,*t2,*t3;
lv_obj_t *list_menu;
lv_obj_t *page_call;
lv_group_t *g;

static void  create_call_ctrl(lv_obj_t *parent);

static void list_menu_btn_event_handler(lv_obj_t *obj, lv_event_t event)
{

    if(event == LV_EVENT_CLICKED){
        printf("Clicked %s\n",lv_list_get_btn_text(obj));
        //printf("clicked btn %d \n", lv_list_get_btn_selected(obj));
        //lv_page_clean();
        int32_t index = lv_list_get_btn_index(list_menu, obj);
        if(index == 0){
            create_call_ctrl(page_call);

        }else if(index == 1){

        }else if(index == 2){

        }

    }
}

#define LIST_MENU_WIDTH   (150)





static void create_list_page(lv_obj_t *parent)
{


    lv_page_set_scrl_layout(parent, LV_LAYOUT_ROW_TOP);
    // create list
    list_menu = lv_list_create(parent, NULL);
    lv_coord_t pad_y = lv_obj_get_y(list_menu) * 2;
    lv_coord_t pad_x = lv_obj_get_x(list_menu) * 3;
    printf("pad_x:%d,pad_y:%d\n",pad_x,pad_y);

    lv_obj_set_width(list_menu, LIST_MENU_WIDTH);
    lv_obj_set_height(list_menu, lv_obj_get_height_margin(parent) - pad_y);

    // add buttons to the list
    lv_obj_t *list_btn;
    list_btn = lv_list_add_btn(list_menu, LV_SYMBOL_FILE, "呼叫控制");
    lv_obj_set_event_cb(list_btn, list_menu_btn_event_handler);

    list_btn = lv_list_add_btn(list_menu, LV_SYMBOL_DIRECTORY, "常用控制");
    lv_obj_set_event_cb(list_btn, list_menu_btn_event_handler);

    list_btn = lv_list_add_btn(list_menu, LV_SYMBOL_CLOSE, "会议控制");
    lv_obj_set_event_cb(list_btn, list_menu_btn_event_handler);

    list_btn = lv_list_add_btn(list_menu, LV_SYMBOL_EDIT, "流媒体控制");
    lv_obj_set_event_cb(list_btn, list_menu_btn_event_handler);
#if 0
    list_btn = lv_list_add_btn(list_menu, LV_SYMBOL_SAVE, "Save");
    lv_obj_set_event_cb(list_btn, list_menu_btn_event_handler);

    list_btn = lv_list_add_btn(list_menu, LV_SYMBOL_BELL, "Notify");
    lv_obj_set_event_cb(list_btn, list_menu_btn_event_handler);

    list_btn = lv_list_add_btn(list_menu, LV_SYMBOL_CALL, "Call");
    lv_obj_set_event_cb(list_btn, list_menu_btn_event_handler);
#endif
    // create a page
    page_call = lv_page_create(parent, NULL);

    //lv_obj_t *page = lv_cont_create(parent, NULL);
    lv_obj_set_height(page_call, lv_obj_get_height_margin(parent) - pad_y );
    lv_obj_set_width(page_call, lv_obj_get_width_fit(parent) - lv_obj_get_width_fit(list_menu) - pad_x);

    //printf("height:%d,height_margin=%d,height_fit=%d\n",lv_obj_get_width(list_menu),lv_obj_get_width_margin(list_menu),lv_obj_get_width_fit(list_menu));

}

static void  create_call_ctrl(lv_obj_t *parent)
{

#define CONT_LINE_WIDTH (350)
    lv_page_clean(parent);

    //lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_MID);

    //lv_obj_t *sw_manu = lv_switch_create(parent, NULL);

    lv_obj_t *cont_manu = lv_cont_create(parent, NULL);


    lv_cont_set_layout(cont_manu, LV_LAYOUT_GRID);
    lv_obj_set_width(cont_manu, 500);
    //lv_obj_set_auto_realign(cont_manu, true);
    //lv_obj_set_height(cont_manu, 250);
    lv_cont_set_fit2(cont_manu, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_align(cont_manu,NULL, LV_ALIGN_IN_TOP_MID, 0,20);
    // container of manu info
    lv_obj_t *label_ip = lv_label_create(cont_manu, NULL);
    lv_label_set_text(label_ip, " IP地址");
    lv_obj_t *text_ip = lv_textarea_create(cont_manu, NULL);
    lv_obj_align(text_ip, NULL, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 20);
    lv_textarea_set_one_line(text_ip, true);
    lv_textarea_set_cursor_hidden(text_ip, true);
    lv_textarea_set_placeholder_text(text_ip, "IP ");
    lv_textarea_set_text(text_ip, "");
    lv_obj_set_width(text_ip, CONT_LINE_WIDTH);

    lv_obj_t *label_speed = lv_label_create(cont_manu, NULL);
    lv_label_set_text(label_speed, "连接速度");
    lv_obj_t *dd_speed =  lv_dropdown_create(cont_manu, NULL);
    lv_obj_set_width(dd_speed, CONT_LINE_WIDTH);
    lv_obj_t *label_video = lv_label_create(cont_manu, NULL);
    lv_label_set_text(label_video, " 视频模式");
    lv_obj_t * dd_video = lv_dropdown_create(cont_manu, NULL);
    lv_obj_set_width(dd_video, CONT_LINE_WIDTH);
    lv_obj_t *label_audio = lv_label_create(cont_manu, NULL);
    lv_label_set_text(label_audio, " 音频模式");
    lv_obj_t * dd_audio = lv_dropdown_create(cont_manu, NULL);
    lv_obj_set_width(dd_audio, CONT_LINE_WIDTH);

    lv_obj_t *cb_manu = lv_checkbox_create(parent, NULL);
    lv_checkbox_set_text_static(cb_manu, "手动输入信息");
    lv_obj_align(cb_manu,cont_manu, LV_ALIGN_OUT_TOP_LEFT, 10, 10);
#if 1
    //lv_obj_t *sw_addr = lv_switch_create(parent, NULL);

    lv_obj_t *tb_addr = lv_table_create(parent, NULL);
    lv_obj_set_width(tb_addr, 500);
    lv_obj_set_height(tb_addr, 100);
    lv_obj_align(tb_addr, cont_manu, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 30);
    lv_obj_t *cb_addr = lv_checkbox_create(parent, NULL);
    lv_checkbox_set_text_static(cb_addr, "使用地址薄");
    lv_obj_align(cb_addr,tb_addr,LV_ALIGN_OUT_TOP_LEFT,10,10);
    lv_obj_t *btn_call = lv_btn_create(parent, NULL);
    lv_obj_t * label_call = lv_label_create(btn_call, NULL);
    lv_label_set_text(label_call, "呼叫");
    lv_obj_align(btn_call, tb_addr, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    lv_obj_t *btn_second = lv_btn_create(parent, NULL);
    lv_obj_align(btn_second, btn_call, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_t * label_second = lv_label_create(btn_second, NULL);
    lv_label_set_text(label_second, "呼叫第二流");
#endif


}


static void tv_event_cb(lv_obj_t * ta, lv_event_t e)
{

    if(e == LV_EVENT_VALUE_CHANGED || e == LV_EVENT_REFRESH){
        lv_group_remove_all_objs(g);

        uint16_t tab = lv_tabview_get_tab_act(tv);
        lv_group_add_obj(g, tv);

        if(tab == 0){
            lv_group_add_obj(g, list_menu);

        }else if(tab == 1){

        }else if(tab == 2){

        }


        //uint32_t i;

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

lv_obj_t *create_setting_panel(lv_obj_t *parent,lv_group_t *group)
{



    g = group;
    lv_group_set_focus_cb(g, focus_cb);
// https://docs.lvgl.io/v7/en/html/overview/display.html#transparent-screens
    lv_obj_set_style_local_bg_opa(lv_scr_act(),LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_disp_set_bg_opa(NULL, LV_OPA_TRANSP);

    tv = lv_tabview_create(parent, NULL);
    lv_obj_set_size(tv, PANEL_WIDTH, PANEL_HEIGH);
    lv_obj_set_event_cb(tv, tv_event_cb);

    lv_obj_align(tv, NULL, LV_ALIGN_CENTER, 0, 0);
    t1 = lv_tabview_add_tab(tv, "视频会议");
    t2 = lv_tabview_add_tab(tv, "系统设置");
    t3 = lv_tabview_add_tab(tv, "系统诊断");
    lv_group_add_obj(group, tv);

    create_list_page(t1);


    return tv;
}
