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

#include "keypad.h"
#include "ui_fd.h"
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


    gsf_mpp_fb_start(VOFB_GUI, VO_OUTPUT_1080P30, 1);//first hide ui

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
