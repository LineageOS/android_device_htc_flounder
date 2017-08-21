/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef __TEGRA_DC_EXT_H
#define __TEGRA_DC_EXT_H
#include <linux/types.h>
#include <linux/ioctl.h>
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#include <time.h>
#include <unistd.h>
#define TEGRA_DC_EXT_FMT_P1 0
#define TEGRA_DC_EXT_FMT_P2 1
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FMT_P4 2
#define TEGRA_DC_EXT_FMT_P8 3
#define TEGRA_DC_EXT_FMT_B4G4R4A4 4
#define TEGRA_DC_EXT_FMT_B5G5R5A 5
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FMT_B5G6R5 6
#define TEGRA_DC_EXT_FMT_AB5G5R5 7
#define TEGRA_DC_EXT_FMT_B8G8R8A8 12
#define TEGRA_DC_EXT_FMT_R8G8B8A8 13
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FMT_B6x2G6x2R6x2A8 14
#define TEGRA_DC_EXT_FMT_R6x2G6x2B6x2A8 15
#define TEGRA_DC_EXT_FMT_YCbCr422 16
#define TEGRA_DC_EXT_FMT_YUV422 17
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FMT_YCbCr420P 18
#define TEGRA_DC_EXT_FMT_YUV420P 19
#define TEGRA_DC_EXT_FMT_YCbCr422P 20
#define TEGRA_DC_EXT_FMT_YUV422P 21
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FMT_YCbCr422R 22
#define TEGRA_DC_EXT_FMT_YUV422R 23
#define TEGRA_DC_EXT_FMT_YCbCr422RA 24
#define TEGRA_DC_EXT_FMT_YUV422RA 25
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FMT_YCbCr444P 41
#define TEGRA_DC_EXT_FMT_YUV444P 52
#define TEGRA_DC_EXT_FMT_YCrCb420SP 42
#define TEGRA_DC_EXT_FMT_YCbCr420SP 43
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FMT_YCrCb422SP 44
#define TEGRA_DC_EXT_FMT_YCbCr422SP 45
#define TEGRA_DC_EXT_FMT_YVU420SP 53
#define TEGRA_DC_EXT_FMT_YUV420SP 54
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FMT_YVU422SP 55
#define TEGRA_DC_EXT_FMT_YUV422SP 56
#define TEGRA_DC_EXT_FMT_YVU444SP 59
#define TEGRA_DC_EXT_FMT_YUV444SP 60
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FMT_SHIFT 0
#define TEGRA_DC_EXT_FMT_MASK (0xff << TEGRA_DC_EXT_FMT_SHIFT)
#define TEGRA_DC_EXT_FMT_BYTEORDER_NOSWAP (0 << 8)
#define TEGRA_DC_EXT_FMT_BYTEORDER_SWAP2 (1 << 8)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FMT_BYTEORDER_SWAP4 (2 << 8)
#define TEGRA_DC_EXT_FMT_BYTEORDER_SWAP4HW (3 << 8)
#define TEGRA_DC_EXT_FMT_BYTEORDER_SWAP02 (4 << 8)
#define TEGRA_DC_EXT_FMT_BYTEORDER_SWAPLEFT (5 << 8)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FMT_BYTEORDER_SHIFT 8
#define TEGRA_DC_EXT_FMT_BYTEORDER_MASK (0x0f << TEGRA_DC_EXT_FMT_BYTEORDER_SHIFT)
#define TEGRA_DC_EXT_BLEND_NONE 0
#define TEGRA_DC_EXT_BLEND_PREMULT 1
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_BLEND_COVERAGE 2
#define TEGRA_DC_EXT_FLIP_FLAG_INVERT_H (1 << 0)
#define TEGRA_DC_EXT_FLIP_FLAG_INVERT_V (1 << 1)
#define TEGRA_DC_EXT_FLIP_FLAG_TILED (1 << 2)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FLIP_FLAG_CURSOR (1 << 3)
#define TEGRA_DC_EXT_FLIP_FLAG_GLOBAL_ALPHA (1 << 4)
#define TEGRA_DC_EXT_FLIP_FLAG_BLOCKLINEAR (1 << 5)
#define TEGRA_DC_EXT_FLIP_FLAG_SCAN_COLUMN (1 << 6)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FLIP_FLAG_INTERLACE (1 << 7)
#define TEGRA_DC_EXT_FLIP_FLAG_UPDATE_CSC (1 << 9)
struct tegra_timespec {
  __s32 tv_sec;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __s32 tv_nsec;
};
struct tegra_dc_ext_flip_windowattr {
  __s32 index;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 buff_id;
  __u32 blend;
  __u32 offset;
  __u32 offset_u;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 offset_v;
  __u32 stride;
  __u32 stride_uv;
  __u32 pixformat;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 x;
  __u32 y;
  __u32 w;
  __u32 h;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 out_x;
  __u32 out_y;
  __u32 out_w;
  __u32 out_h;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 z;
  __u32 swap_interval;
  struct tegra_timespec timestamp;
  union {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
    struct {
      __u32 pre_syncpt_id;
      __u32 pre_syncpt_val;
    };
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
    __s32 pre_syncpt_fd;
  };
  __u32 buff_id_u;
  __u32 buff_id_v;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 flags;
  __u8 global_alpha;
  __u8 block_height_log2;
  __u8 pad1[2];
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  union {
    struct {
      __u32 offset2;
      __u32 offset_u2;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
      __u32 offset_v2;
      __u32 pad2[1];
    };
    struct {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
      __u16 yof;
      __u16 kyrgb;
      __u16 kur;
      __u16 kvr;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
      __u16 kug;
      __u16 kvg;
      __u16 kub;
      __u16 kvb;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
    } csc;
  };
};
#define TEGRA_DC_EXT_FLIP_N_WINDOWS 3
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct tegra_dc_ext_flip {
  struct tegra_dc_ext_flip_windowattr win[TEGRA_DC_EXT_FLIP_N_WINDOWS];
  __u32 post_syncpt_id;
  __u32 post_syncpt_val;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
struct tegra_dc_ext_flip_2 {
  struct tegra_dc_ext_flip_windowattr * win;
  __u8 win_num;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u8 reserved1;
  __u16 reserved2;
  __u32 post_syncpt_id;
  __u32 post_syncpt_val;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u16 dirty_rect[4];
};
struct tegra_dc_ext_flip_3 {
  __u64 win;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u8 win_num;
  __u8 reserved1;
  __u16 reserved2;
  __s32 post_syncpt_fd;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u16 dirty_rect[4];
};
struct tegra_dc_ext_set_vblank {
  __u8 enable;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u8 reserved[3];
};
#define TEGRA_DC_EXT_CURSOR_IMAGE_FLAGS_SIZE_32x32 ((1 & 0x7) << 0)
#define TEGRA_DC_EXT_CURSOR_IMAGE_FLAGS_SIZE_64x64 ((2 & 0x7) << 0)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_CURSOR_IMAGE_FLAGS_SIZE_128x128 ((3 & 0x7) << 0)
#define TEGRA_DC_EXT_CURSOR_IMAGE_FLAGS_SIZE_256x256 ((4 & 0x7) << 0)
#define TEGRA_DC_EXT_CURSOR_IMAGE_FLAGS_SIZE(x) (((x) & 0x7) >> 0)
#define TEGRA_DC_EXT_CURSOR_FORMAT_2BIT_LEGACY (0)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_CURSOR_FORMAT_RGBA_NON_PREMULT_ALPHA (1)
#define TEGRA_DC_EXT_CURSOR_FORMAT_RGBA_PREMULT_ALPHA (3)
#define TEGRA_DC_EXT_CURSOR_FORMAT_FLAGS_2BIT_LEGACY (TEGRA_DC_EXT_CURSOR_FORMAT_2BIT_LEGACY << 16)
#define TEGRA_DC_EXT_CURSOR_FORMAT_FLAGS_RGBA_NON_PREMULT_ALPHA (TEGRA_DC_EXT_CURSOR_FORMAT_RGBA_NON_PREMULT_ALPHA << 16)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_CURSOR_FORMAT_FLAGS_RGBA_PREMULT_ALPHA (TEGRA_DC_EXT_CURSOR_FORMAT_RGBA_PREMULT_ALPHA << 16)
#define TEGRA_DC_EXT_CURSOR_FORMAT_FLAGS(x) (((x) >> 16) & 0x7)
#define TEGRA_DC_EXT_CURSOR_FLAGS_RGBA_NORMAL TEGRA_DC_EXT_CURSOR_FORMAT_FLAGS_RGBA_NON_PREMULT_ALPHA
#define TEGRA_DC_EXT_CURSOR_FLAGS_2BIT_LEGACY TEGRA_DC_EXT_CURSOR_FORMAT_FLAGS_2BIT_LEGACY
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
enum CR_MODE {
  legacy,
  normal,
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct tegra_dc_ext_cursor_image {
  struct {
    __u8 r;
    __u8 g;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
    __u8 b;
  } foreground, background;
  __u32 buff_id;
  __u32 flags;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __s16 x;
  __s16 y;
  __u32 vis;
  enum CR_MODE mode;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
#define TEGRA_DC_EXT_CURSOR_FLAGS_VISIBLE (1 << 0)
struct tegra_dc_ext_cursor {
  __s16 x;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __s16 y;
  __u32 flags;
};
struct tegra_dc_ext_csc {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 win_index;
  __u16 yof;
  __u16 kyrgb;
  __u16 kur;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u16 kvr;
  __u16 kug;
  __u16 kvg;
  __u16 kub;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u16 kvb;
};
struct tegra_dc_ext_cmu {
  __u16 cmu_enable;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u16 csc[9];
  __u16 lut1[256];
  __u16 lut2[960];
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct tegra_dc_ext_lut {
  __u32 win_index;
  __u32 flags;
  __u32 start;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 len;
  __u16 * r;
  __u16 * g;
  __u16 * b;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
#define TEGRA_DC_EXT_LUT_FLAGS_FBOVERRIDE 0x01
#define TEGRA_DC_EXT_FLAGS_ENABLED 1
struct tegra_dc_ext_status {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 flags;
  __u32 pad[3];
};
struct tegra_dc_ext_feature {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 length;
  __u32 * entries;
};
#define TEGRA_DC_EXT_SET_NVMAP_FD _IOW('D', 0x00, __s32)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_GET_WINDOW _IOW('D', 0x01, __u32)
#define TEGRA_DC_EXT_PUT_WINDOW _IOW('D', 0x02, __u32)
#define TEGRA_DC_EXT_FLIP _IOWR('D', 0x03, struct tegra_dc_ext_flip)
#define TEGRA_DC_EXT_GET_CURSOR _IO('D', 0x04)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_PUT_CURSOR _IO('D', 0x05)
#define TEGRA_DC_EXT_SET_CURSOR_IMAGE _IOW('D', 0x06, struct tegra_dc_ext_cursor_image)
#define TEGRA_DC_EXT_SET_CURSOR _IOW('D', 0x07, struct tegra_dc_ext_cursor)
#define TEGRA_DC_EXT_SET_CSC _IOW('D', 0x08, struct tegra_dc_ext_csc)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_GET_STATUS _IOR('D', 0x09, struct tegra_dc_ext_status)
#define TEGRA_DC_EXT_GET_VBLANK_SYNCPT _IOR('D', 0x09, __u32)
#define TEGRA_DC_EXT_SET_LUT _IOW('D', 0x0A, struct tegra_dc_ext_lut)
#define TEGRA_DC_EXT_GET_FEATURES _IOW('D', 0x0B, struct tegra_dc_ext_feature)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_CURSOR_CLIP _IOW('D', 0x0C, __s32)
#define TEGRA_DC_EXT_SET_CMU _IOW('D', 0x0D, struct tegra_dc_ext_cmu)
#define TEGRA_DC_EXT_FLIP2 _IOWR('D', 0x0E, struct tegra_dc_ext_flip_2)
#define TEGRA_DC_EXT_GET_CMU _IOR('D', 0x0F, struct tegra_dc_ext_cmu)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_GET_CUSTOM_CMU _IOR('D', 0x10, struct tegra_dc_ext_cmu)
#define TEGRA_DC_EXT_SET_CURSOR_IMAGE_LOW_LATENCY _IOW('D', 0x11, struct tegra_dc_ext_cursor_image)
#define TEGRA_DC_EXT_SET_CURSOR_LOW_LATENCY _IOW('D', 0x12, struct tegra_dc_ext_cursor_image)
#define TEGRA_DC_EXT_SET_PROPOSED_BW _IOR('D', 0x13, struct tegra_dc_ext_flip_2)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_FLIP3 _IOWR('D', 0x14, struct tegra_dc_ext_flip_3)
#define TEGRA_DC_EXT_SET_VBLANK _IOW('D', 0x15, struct tegra_dc_ext_set_vblank)
enum tegra_dc_ext_control_output_type {
  TEGRA_DC_EXT_DSI,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  TEGRA_DC_EXT_LVDS,
  TEGRA_DC_EXT_VGA,
  TEGRA_DC_EXT_HDMI,
  TEGRA_DC_EXT_DVI,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  TEGRA_DC_EXT_DP,
};
struct tegra_dc_ext_control_output_properties {
  __u32 handle;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  enum tegra_dc_ext_control_output_type type;
  __u32 connected;
  __s32 associated_head;
  __u32 head_mask;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
struct tegra_dc_ext_control_output_edid {
  __u32 handle;
  __u32 size;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  void * data;
};
struct tegra_dc_ext_event {
  __u32 type;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 data_size;
  char data[0];
};
#define TEGRA_DC_EXT_EVENT_HOTPLUG (1 << 0)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct tegra_dc_ext_control_event_hotplug {
  __u32 handle;
};
#define TEGRA_DC_EXT_EVENT_VBLANK (1 << 1)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct tegra_dc_ext_control_event_vblank {
  __u32 handle;
  __u32 reserved;
  __u64 timestamp_ns;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
#define TEGRA_DC_EXT_EVENT_BANDWIDTH_INC (1 << 2)
#define TEGRA_DC_EXT_EVENT_BANDWIDTH_DEC (1 << 3)
struct tegra_dc_ext_control_event_bandwidth {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 handle;
  __u32 total_bw;
  __u32 avail_bw;
  __u32 resvd_bw;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
#define TEGRA_DC_EXT_CAPABILITIES_CURSOR_MODE (1 << 0)
#define TEGRA_DC_EXT_CAPABILITIES_BLOCKLINEAR (1 << 1)
#define TEGRA_DC_EXT_CAPABILITIES_CURSOR_TWO_COLOR (1 << 2)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_CAPABILITIES_CURSOR_RGBA_NON_PREMULT_ALPHA (1 << 3)
#define TEGRA_DC_EXT_CAPABILITIES_CURSOR_RGBA_PREMULT_ALPHA (1 << 4)
struct tegra_dc_ext_control_capabilities {
  __u32 caps;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 pad[3];
};
#define TEGRA_DC_EXT_CONTROL_GET_NUM_OUTPUTS _IOR('C', 0x00, __u32)
#define TEGRA_DC_EXT_CONTROL_GET_OUTPUT_PROPERTIES _IOWR('C', 0x01, struct tegra_dc_ext_control_output_properties)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_DC_EXT_CONTROL_GET_OUTPUT_EDID _IOWR('C', 0x02, struct tegra_dc_ext_control_output_edid)
#define TEGRA_DC_EXT_CONTROL_SET_EVENT_MASK _IOW('C', 0x03, __u32)
#define TEGRA_DC_EXT_CONTROL_GET_CAPABILITIES _IOR('C', 0x04, struct tegra_dc_ext_control_capabilities)
#endif
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */

