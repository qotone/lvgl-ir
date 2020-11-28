#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>

#include "hi_comm_video.h"
#include "hi_defines.h"
#include "hi_type.h"
#include "hink_comm.h"
#include "hink_sys.h"
#include "hink_vo.h"
#include "hink_vi.h"
#include "hink_vpss.h"
#include "hifb.h"
#include "keypad.h"
//vofb;
enum {
    VOFB_GUI   = 0, // GUI Layer
    VOFB_MOUSE = 1, // MOUSE Layer
    VOFB_BUTT
};

extern int lvgl_stop(void);
extern int lvgl_start(int w, int h);

volatile static int isRunning  = 0;

VI_DEV viDev = 6;
VI_CHN viChn;

static void APP_HandleSig(HI_S32 signo)
{
    printf("signo = %d\n",signo );
    if(SIGINT == signo || SIGTERM == signo || SIGKILL == signo){

        isRunning  = 0;


    }
}

static struct fb_bitfield s_r16 = {10, 5, 0};
static struct fb_bitfield s_g16 = {5, 5, 0};
static struct fb_bitfield s_b16 = {0, 5, 0};
static struct fb_bitfield s_a16 = {15, 1, 0};

static struct fb_bitfield s_a32 = {24,8,0};
static struct fb_bitfield s_r32 = {16,8,0};
static struct fb_bitfield s_g32 = {8,8,0};
static struct fb_bitfield s_b32 = {0,8,0};

static int vo_fd[VOFB_BUTT];

static int gsf_mpp_fb_start(int vofb, VO_INTF_SYNC_E sync, int hide)
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

static int gsf_mpp_fb_hide(int vofb, int hide)
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

static HI_S32 test_vo_start(HI_VOID)
{
    VO_PUB_ATTR_S stVoAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VO_CHN voChn = 0;
    HINK_RECT_S stRect = {.x = 0, .y = 0, .w = 1920, .h = 1080};

    HI_S32 s32Ret = HI_SUCCESS;
    stVoAttr.enIntfSync = VO_OUTPUT_1080P30;
    stVoAttr.enIntfType = VO_INTF_HDMI;
    stVoAttr.u32BgColor = 0xFFFFFF;

    memset(&(stLayerAttr), 0 , sizeof(VO_VIDEO_LAYER_ATTR_S));
    stLayerAttr.bClusterMode = HI_FALSE;
    stLayerAttr.bDoubleFrame = HI_FALSE;
	s32Ret = hink_vo_getWH(stVoAttr.enIntfSync,&stLayerAttr.stImageSize.u32Width,&stLayerAttr.stImageSize.u32Height, &stLayerAttr.u32DispFrmRt);
	if (HI_SUCCESS != s32Ret){
		HINK_PRT("hink_vo_getWH failed!\n");
		return HI_FAILURE;
	}

	stLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    stLayerAttr.stDispRect.s32X       = 0;
    stLayerAttr.stDispRect.s32Y       = 0;
    stLayerAttr.stDispRect.u32Width   = stLayerAttr.stImageSize.u32Width;
    stLayerAttr.stDispRect.u32Height  = stLayerAttr.stImageSize.u32Height;


    printf(" ... init with HDMI mode...\n");


    s32Ret = hink_vo_startDev(HINK_VO_DEV_DHD0, &stVoAttr);

    s32Ret = hink_vo_startLayer(HINK_VO_LAYER_VHD0,&stLayerAttr);

    s32Ret = hink_vo_startChn(HINK_VO_LAYER_VHD0,voChn,&stRect);


    s32Ret = hink_vo_startHdmi(stVoAttr.enIntfSync, HI_TRUE);


    return s32Ret;

}

static HI_S32 test_vo_stop(HI_VOID)
{
    VO_CHN voChn = 0;
    HI_S32 s32Ret = HI_FAILURE;
    printf("... uninit vo ....\n");

    s32Ret = hink_vo_stopChn(HINK_VO_LAYER_VHD0, voChn);

    s32Ret = hink_vo_stopLayer(HINK_VO_LAYER_VHD0);

    s32Ret = hink_vo_stopDev(HINK_VO_DEV_DHD0);


    s32Ret = hink_vo_stopHdmi();


    return s32Ret;

}

static HI_S32 test_vi_start()
{
    HI_S32 s32Ret = 0;
    VI_PARAM_S viParam;
    hink_vi_getParam(VI_MODE_8_1080P,&viParam);
    viChn = viParam.s32ViChnInterval * viDev;
    s32Ret = hink_vi_init(viDev,viChn,PIC_HD1080);

    return s32Ret;
}

static HI_S32 test_vi_stop()
{

    return 0;
}

int main(int argc, char *argv[])
{

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize = 1920*1080*2;
    VIDEO_NORM_E enNorm = VIDEO_ENCODING_MODE_AUTO;


    signal(SIGINT,APP_HandleSig);
    signal(SIGTERM, APP_HandleSig);

    u32BlkSize = hink_sys_calcPicVbBlkSize(enNorm, PIC_HD1080, HINK_PIXEL_FORMAT, HINK_SYS_ALIGN_WIDTH, COMPRESS_MODE_SEG);
    hink_sys_init(u32BlkSize, 18);

    test_vi_start();
    test_vo_start();
    /* hink_vpss_quickStart(0, VPSS_CHN0, 1920, 1080); */
    hink_sys_voBindVi(HINK_VO_LAYER_VHD0, 0, viDev, viChn);


    gsf_mpp_fb_start(VOFB_GUI, VO_OUTPUT_1080P30, 0);

    lvgl_start(1920, 1080);

    isRunning = 1;

    while (isRunning) {
        sleep(5);
    }

    lvgl_stop();
    test_vo_stop();
    hink_sys_unInit();

    return 0;
}
