#include <asm-generic/errno-base.h>
#include <time.h>
#include <pthread.h>
#include <stddef.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>

#include "keypad.h"



//#include "ui_setting_panel.h"
extern void kp_callback(irkey_info_s irkey);

//extern lv_group_t*  g;



typedef struct {
    fd_set fds;
    int fd_max;
    int listen_fd;
    int should_quit;
    pthread_t pid;
} keypad_cxt_t;

/**********************
 *  STATIC VARIABLES
 **********************/
uint32_t last_key;
lv_indev_state_t state;

static keypad_cxt_t ctx = {0};

static void * keypad_handler(void *p);
static uint32_t keycode_to_ascii(uint32_t ir_key);

/* Initialize your keypad */
void keypad_init(void)
{
    /*Your code comes here*/
    int ir_fd = -1;
    ir_fd = open("/dev/Hi_IR",O_RDWR);
    if(ir_fd < 0){
        printf("ERROR: can't open %s device.\n","/dev/Hi_IR" );
    }
    ctx.listen_fd = ir_fd;
    FD_ZERO(&ctx.fds);
    FD_SET(ctx.listen_fd, &ctx.fds);
    ctx.fd_max = ctx.listen_fd;
    ctx.should_quit = 0;
    pthread_create(&ctx.pid, NULL,keypad_handler, &ctx);
}

void keypad_uninit(void)
{

    ctx.should_quit = 1;
    pthread_join(ctx.pid, NULL);

}

/* Will be called by the library to read the mouse */
bool keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    (void) indev_drv;      /*Unused*/
    data->state = state;
    data->key = keycode_to_ascii(last_key);

    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}


static void * keypad_handler(void *p)
{
    fd_set read_fds;
    int res,count,i;
    irkey_info_s rcv_irkey_info[4];
    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = 2000;


    while(!ctx.should_quit){
        read_fds = ctx.fds;
        if(select(ctx.fd_max + 1,&read_fds,NULL,NULL,&time) < 0){
            if(errno == EINTR){
                continue;
            }else{
                perror("select");
                return (void *)NULL;
            }
        }

        if( FD_ISSET(ctx.listen_fd, &read_fds)){
            res = read(ctx.listen_fd,rcv_irkey_info,sizeof(rcv_irkey_info));
            count = res / sizeof(irkey_info_s);
            if((res > 0) && (res <= sizeof(rcv_irkey_info))){
                for(i = 0; i < count; i++){
                    printf("index:%d\tkeyvalue=H/L/S 0x%lx/0x%lx/0x%lx\n",i,rcv_irkey_info[i].irkey_datah,rcv_irkey_info[i].irkey_datal,rcv_irkey_info[i].irkey_state_code);
#if 0
                    state = rcv_irkey_info[i].irkey_state_code ? LV_INDEV_STATE_REL : LV_INDEV_STATE_PR;
                    if(state == LV_INDEV_STATE_PR){
                        last_key = rcv_irkey_info[i].irkey_datal;//keycode_to_ascii(rcv_irkey_info[i].irkey_datal);
                    }
#endif
                    /* if(act_obj != NULL){ */
                    /*     if(rcv_irkey_info[i].irkey_state_code == 0x01 && rcv_irkey_info[i].irkey_datal == REMOTE_MENU){ */
                    /*         printf("delete panel\n"); */
                    /*         lv_obj_del(act_obj); */
                    /*         act_obj = NULL; */
                    /*     }else{ */
                    /*         state = rcv_irkey_info[i].irkey_state_code ? LV_INDEV_STATE_REL : LV_INDEV_STATE_PR; */
                    /*         if(state == LV_INDEV_STATE_PR){ */
                    /*             last_key = rcv_irkey_info[i].irkey_datal;//keycode_to_ascii(rcv_irkey_info[i].irkey_datal); */
                    /*         } */
                    /*     } */
                    /* }else{ */

                    /*     if(rcv_irkey_info[i].irkey_state_code == 0x01 && rcv_irkey_info[i].irkey_datal == REMOTE_MENU){ */
                    /*         printf("create panel\n"); */
                    /*         act_obj = create_setting_panel(lv_scr_act(), NULL); */
                    /*         printf("create_panel created ok!\n"); */
                    /*     } */
                    /* } */
                    kp_callback(rcv_irkey_info[i]);

                }
            }
            FD_CLR(ctx.listen_fd, &read_fds);
        }

    }

    close(ctx.listen_fd);

    printf("keypad handler exit!\n");
    return (void *)NULL;
}

/*Translate the keys to LVGL control characters according to your key definitions*/
/**
 * Convert the key code LV_KEY_... "codes" or leave them if they are not control characters
 * @param sdl_key the key code
 * @return
 */
static uint32_t keycode_to_ascii(uint32_t ir_key)
{
    /*Remap some key to LV_KEY_... to manage groups*/
    switch(ir_key) {
    case REMOTE_RIGHT_CURSOR:
        return LV_KEY_RIGHT;

    case REMOTE_LEFT_CURSOR:

        return LV_KEY_LEFT;
    case REMOTE_UP_CURSOR:
        return LV_KEY_UP;
    case REMOTE_DOWN_CURSOR:

        return LV_KEY_DOWN;
    case REMOTE_GO_BACK:

        return LV_KEY_ESC;
    case REMOTE_SWITH_1_2:

        return LV_KEY_DEL;
    case REMOTE_BACKSPACE:

        return LV_KEY_DEL;
    case REMOTE_ENTER:
    case '\r':
        return LV_KEY_ENTER;
    case REMOTE_F1:
        return LV_KEY_NEXT;
    case REMOTE_F2:
        return LV_KEY_PREV;

    default:
        //printf("ir_key = 0x%lx\n",ir_key );
        return ir_key;
    }
}
