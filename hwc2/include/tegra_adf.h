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
#ifndef _UAPI_VIDEO_TEGRA_ADF_H_
#define _UAPI_VIDEO_TEGRA_ADF_H_
#include <linux/types.h>
#include <video/adf.h>
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#include <drm/drm_fourcc.h>
#define TEGRA_ADF_FORMAT_P1 fourcc_code('P', '1', ' ', ' ')
#define TEGRA_ADF_FORMAT_P2 fourcc_code('P', '2', ' ', ' ')
#define TEGRA_ADF_FORMAT_P4 fourcc_code('P', '4', ' ', ' ')
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_ADF_FORMAT_P8 fourcc_code('P', '8', ' ', ' ')
#define TEGRA_ADF_FORMAT_B6x2G6x2R6x2A8 fourcc_code('B', 'A', '6', '2')
#define TEGRA_ADF_FORMAT_R6x2G6x2B6x2A8 fourcc_code('R', 'A', '6', '2')
#define TEGRA_ADF_FORMAT_R6x2G6x2B6x2A8 fourcc_code('R', 'A', '6', '2')
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_ADF_FORMAT_YCbCr422R fourcc_code('Y', 'U', '2', 'R')
enum tegra_adf_interface_type {
  TEGRA_ADF_INTF_RGB = ADF_INTF_TYPE_DEVICE_CUSTOM,
  TEGRA_ADF_INTF_LVDS = ADF_INTF_TYPE_DEVICE_CUSTOM + 1,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
#define TEGRA_ADF_EVENT_BANDWIDTH_RENEGOTIATE ADF_EVENT_DEVICE_CUSTOM
struct tegra_adf_event_bandwidth {
  struct adf_event base;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 total_bw;
  __u32 avail_bw;
  __u32 resvd_bw;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_ADF_CAPABILITIES_CURSOR_MODE (1 << 0)
#define TEGRA_ADF_CAPABILITIES_BLOCKLINEAR (1 << 1)
struct tegra_adf_capabilities {
  __u32 caps;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 pad[3];
};
#define TEGRA_ADF_BLEND_NONE 0
#define TEGRA_ADF_BLEND_PREMULT 1
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_ADF_BLEND_COVERAGE 2
#define TEGRA_ADF_FLIP_FLAG_INVERT_H (1 << 0)
#define TEGRA_ADF_FLIP_FLAG_INVERT_V (1 << 1)
#define TEGRA_ADF_FLIP_FLAG_TILED (1 << 2)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_ADF_FLIP_FLAG_CURSOR (1 << 3)
#define TEGRA_ADF_FLIP_FLAG_GLOBAL_ALPHA (1 << 4)
#define TEGRA_ADF_FLIP_FLAG_BLOCKLINEAR (1 << 5)
#define TEGRA_ADF_FLIP_FLAG_SCAN_COLUMN (1 << 6)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define TEGRA_ADF_FLIP_FLAG_INTERLACE (1 << 7)
struct tegra_adf_flip_windowattr {
  __s32 win_index;
  __s32 buf_index;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 blend;
  __u32 x;
  __u32 y;
  __u32 out_x;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 out_y;
  __u32 out_w;
  __u32 out_h;
  __u32 z;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 swap_interval;
  __s64 timestamp_ns;
  __u32 flags;
  __u8 global_alpha;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u8 block_height_log2;
  __u8 pad1[2];
  __u32 offset2;
  __u32 offset_u2;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u32 offset_v2;
  __u32 pad2[1];
};
struct tegra_adf_flip {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  __u8 win_num;
  __u8 reserved1;
  __u16 reserved2;
  __u16 dirty_rect[4];
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  struct tegra_adf_flip_windowattr win[0];
};
struct tegra_adf_proposed_bw {
  __u8 win_num;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  struct {
    __u32 format;
    __u32 w;
    __u32 h;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
    struct tegra_adf_flip_windowattr attr;
  } win[0];
};
#define TEGRA_ADF_SET_PROPOSED_BW _IOW(ADF_IOCTL_TYPE, ADF_IOCTL_NR_CUSTOM, struct tegra_adf_proposed_bw)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
enum {
  TEGRA_DC_Y,
  TEGRA_DC_U,
  TEGRA_DC_V,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
  TEGRA_DC_NUM_PLANES,
};
#endif

