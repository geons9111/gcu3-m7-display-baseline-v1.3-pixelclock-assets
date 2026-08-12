#pragma once
/*
 * dpu_driver.h
 *
 * GCU3 M7 — DPU (Display Processing Unit) ドライバー。
 * ARCH-005 境界: display_engine/ はこのドライバーのみを呼び出します。他のレイヤーはDPUレジスタに直接アクセスしません (ベースライン v1.1 §12)。
 */

#include <stdint.h>

typedef struct
{
    uint16_t width_px;
    uint16_t height_px;
    uint32_t pixel_format;   /* fsl_dpu.hの kDPU_PixelFormatRGB565 などにマッピング */
    uint32_t framebuffer_addr; /* OCRAM/DDR フレームバッファの物理アドレス */
} dpu_driver_config_t;

int  dpu_driver_init(const dpu_driver_config_t *config);
int  dpu_driver_start(void);
void dpu_driver_stop(void);

/* frame_id: シンボリックバッファセレクタ（"logo" / "safe" / 将来のダブルバッファインデックス）。
 * アクティブプレーンのソースアドレスをスワップします。MIPI-DSI自体にはアクセスしません（それは display_link/mipi_dsi の役割です）。 */
void dpu_driver_submit_frame(const char *frame_id);
