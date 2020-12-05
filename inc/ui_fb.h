#ifndef __UI_FB_H
#define __UI_FB_H

#include "hi_comm_video.h"
#include "hi_defines.h"
#include "hi_type.h"
#include "hifb.h"
#include "hi_comm_vo.h"

//vofb;
enum {
    VOFB_GUI   = 0, // GUI Layer
    VOFB_MOUSE = 1, // MOUSE Layer
    VOFB_BUTT
};


int gsf_mpp_fb_start(int vofb, VO_INTF_SYNC_E sync, int hide);

int gsf_mpp_fb_hide(int vofb, int hide);

#endif
