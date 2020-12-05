#include "ui_fb.h"

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>

static struct fb_bitfield s_r16 = {10, 5, 0};
static struct fb_bitfield s_g16 = {5, 5, 0};
static struct fb_bitfield s_b16 = {0, 5, 0};
static struct fb_bitfield s_a16 = {15, 1, 0};

static struct fb_bitfield s_a32 = {24,8,0};
static struct fb_bitfield s_r32 = {16,8,0};
static struct fb_bitfield s_g32 = {8,8,0};
static struct fb_bitfield s_b32 = {0,8,0};

static int vo_fd[VOFB_BUTT];

int gsf_mpp_fb_start(int vofb, VO_INTF_SYNC_E sync, int hide)
{
    HI_CHAR file[32] = "/dev/fb0";


    switch (vofb)
    {
    case VOFB_GUI:
        strcpy(file, "/dev/fb0");
        break;
    case VOFB_MOUSE:
        strcpy(file, "/dev/fb1");
        break;
    default:
        strcpy(file, "/dev/fb0");
        break;
    }

    /* 1. open framebuffer device overlay 0 */
    int fd = open(file, O_RDWR, 0);
    if(fd < 0)
    {
        printf("open %s failed!\n",file);
        return -1;
    }

    HI_BOOL bShow = HI_FALSE;
    if (ioctl(fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        printf("FBIOPUT_SHOW_HIFB failed!\n");
        return -1;
    }

    /* 2. set the screen original position */
    HIFB_POINT_S stPoint = {0, 0};
    stPoint.s32XPos = 0;
    stPoint.s32YPos = 0;

    if (ioctl(fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        printf("set screen original show position failed!\n");
        close(fd);
        return -1;
    }

    /* 3. get the variable screen info */
    struct fb_var_screeninfo var;
    if (ioctl(fd, FBIOGET_VSCREENINFO, &var) < 0)
    {
        printf("Get variable screen info failed!\n");
        close(fd);
        return -1;
    }

    unsigned int u32Width = 1920;
    unsigned int u32Height = 1080;
    unsigned int u32VoFrmRate = 30;
    hink_vo_getWH(sync, &u32Width, &u32Height, &u32VoFrmRate);
    printf("vofb:%d, u32Width:%d, u32Height:%d\n", vofb, u32Width, u32Height);
    switch (vofb)
    {
    case VOFB_GUI:
        var.xres_virtual = u32Width;
        var.yres_virtual = u32Height;//u32Height*2;
        var.xres = u32Width;
        var.yres = u32Height;
        break;
    case VOFB_MOUSE:
        var.xres_virtual = 48;
        var.yres_virtual = 48;
        var.xres = 48;
        var.yres = 48;
        break;
    default:
        break;
    }

    var.transp= s_a32;
    var.red   = s_r32;
    var.green = s_g32;
    var.blue  = s_b32;
    var.bits_per_pixel = 32;
    var.activate = FB_ACTIVATE_NOW;

    /* 5. set the variable screeninfo */
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &var) < 0)
    {
        printf("Put variable screen info failed!\n");
        close(fd);
        return -1;
    }

    /* 6. get the fix screen info */
    struct fb_fix_screeninfo fix;
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
        printf("Get fix screen info failed!\n");
        close(fd);
        return -1;
    }
    int u32FixScreenStride = fix.line_length;   /*fix screen stride*/

    HIFB_ALPHA_S stAlpha={0};
    if (ioctl(fd, FBIOGET_ALPHA_HIFB,  &stAlpha))
    {
        printf("Get alpha failed!\n");
        close(fd);
        return -1;
    }
    stAlpha.bAlphaEnable = HI_TRUE;
    stAlpha.bAlphaChannel = HI_TRUE;//HI_FALSE;
    stAlpha.u8Alpha0 = 0xff; // 当最高位为0时,选择该值作为Alpha
    stAlpha.u8Alpha1 = 0x0; // 当最高位为1时,选择该值作为Alpha
    stAlpha.u8GlobalAlpha = 0xcf;//在Alpha通道使能时起作用

    if (ioctl(fd, FBIOPUT_ALPHA_HIFB,  &stAlpha) < 0)
    {
        printf("Set alpha failed!\n");
        close(fd);
        return -1;
    }

    HIFB_COLORKEY_S stColorKey;
    stColorKey.bKeyEnable = HI_FALSE;
    stColorKey.u32Key = 0x0000;
    if (ioctl(fd, FBIOPUT_COLORKEY_HIFB, &stColorKey) < 0)
    {
        printf("FBIOPUT_COLORKEY_HIFB failed!\n");
        close(fd);
        return -1;
    }

    /* 7. map the physical video memory for user use */
    HI_U8 *pShowScreen = mmap(HI_NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == pShowScreen)
    {
        printf("mmap framebuffer failed!\n");
        close(fd);
        return -1;
    }
    memset(pShowScreen, 0x00, fix.smem_len);
    munmap(pShowScreen, fix.smem_len);

    if(!hide)
    {
        bShow = HI_TRUE;
        if (ioctl(fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
        {
            printf("FBIOPUT_SHOW_HIFB failed!\n");
            close(fd);
            return -1;
        }
    }

    vo_fd[vofb] = fd;//close(fd);
    return 0;
}

int gsf_mpp_fb_hide(int vofb, int hide)
{
    int fd = vo_fd[vofb];

    if(fd <= 0)
        return -1;

    HI_BOOL bShow = (hide)?HI_FALSE:HI_TRUE;

    if (ioctl(fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        printf("FBIOPUT_SHOW_HIFB failed!\n");
        return -1;
    }

    return 0;
}
