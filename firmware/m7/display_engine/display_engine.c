#include "display_engine.h"
#include "dpu_driver.h"
#include "mipi_dsi_hal.h"
#include "display_config.h"

/* HS bitrate / pixel format are left as explicit named constants here
 * (not yet in display_config.h) pending §21/GATE-07-style confirmation
 * against the actual DSI-to-SerDes bridge datasheet (fpd_link_adapter).
 * DS90UB941AS-Q1 supports up to 4 DSI lanes / 1.5Gbps per lane; 4-lane
 * config chosen to leave headroom for the 1280x480 target in
 * display_config.h without needing compression. */
#define GCU3_DSI_LANE_COUNT      4U
#define GCU3_DSI_HS_BITRATE_BPS  1500000000U
#define GCU3_DPU_PIXEL_FORMAT_RGB565 0U /* placeholder value; map to real
                                            fsl_dpu.h enum at integration time */
#define GCU3_FRAMEBUFFER_ADDR    0x20400000U /* OCRAM region, TBD confirm vs. RDC/TRDC map */

void display_engine_init(void)
{
    dpu_driver_config_t dpu_cfg = {
        .width_px         = GCU3_DISPLAY_WIDTH,
        .height_px        = GCU3_DISPLAY_HEIGHT,
        .pixel_format     = GCU3_DPU_PIXEL_FORMAT_RGB565,
        .framebuffer_addr = GCU3_FRAMEBUFFER_ADDR
    };
    (void)dpu_driver_init(&dpu_cfg);

    mipi_dsi_hal_config_t dsi_cfg = {
        .width_px       = GCU3_DISPLAY_WIDTH,
        .height_px      = GCU3_DISPLAY_HEIGHT,
        .lane_count      = GCU3_DSI_LANE_COUNT,
        .hs_bitrate_bps  = GCU3_DSI_HS_BITRATE_BPS,
        .pixel_clock_hz  = GCU3_DISPLAY_PIXEL_CLOCK_HZ
    };
    (void)mipi_dsi_hal_init(&dsi_cfg);
}

void display_engine_start(void)
{
    (void)dpu_driver_start();
    mipi_dsi_hal_start();
}

void display_engine_render_logo(void)
{
    dpu_driver_submit_frame("logo");
}

void display_engine_render_safe(void)
{
    dpu_driver_submit_frame("safe");
}
