#include "display_engine.h"
#include "dpu_driver.h"
#include "mipi_dsi_hal.h"
#include "display_config.h"

/* HSビットレート/ピクセルフォーマットは、実際のDSIからSerDesへのブリッジデータシート(fpd_link_adapter)に対する§21/GATE-07スタイルの確認が保留中のため、
 * ここでは明示的な名前付き定数として残されています(display_config.hにはまだありません)。
 * DS90UB941AS-Q1は、レーンあたり最大1.5Gbpsの4つのDSIレーンをサポートします。
 * 圧縮を必要とせずにdisplay_config.hの1280x480ターゲットのヘッドルームを残すために、4レーン設定が選択されました。 */
#define GCU3_DSI_HS_BITRATE_BPS  1500000000U
#define GCU3_DPU_PIXEL_FORMAT_RGB565 0U /* プレースホルダー値。統合時に実際の
                                            fsl_dpu.hの列挙型にマッピングします */

extern const uint8_t g_gcu3_logo_framebuffer[];

void display_engine_init(void)
{
    if (GCU3_DISPLAY_PIXEL_CLOCK_HZ == 0U) {
        /* GATE-PCLK-01に基づくフェイルクローズ: 無効なクロックでHWを構成しないでください */
        return;
    }

    dpu_driver_config_t dpu_cfg = {
        .width_px         = GCU3_DISPLAY_WIDTH,
        .height_px        = GCU3_DISPLAY_HEIGHT,
        .pixel_format     = GCU3_DPU_PIXEL_FORMAT_RGB565,
        .framebuffer_addr = (uint32_t)(uintptr_t)g_gcu3_logo_framebuffer
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
