#pragma once
/*
 * dpu_driver.h
 *
 * GCU3 M7 — DPU (Display Processing Unit) driver.
 * ARCH-005 boundary: display_engine/ calls only this driver; no other
 * layer touches DPU registers directly (baseline v1.1 §12).
 */

#include <stdint.h>

typedef struct
{
    uint16_t width_px;
    uint16_t height_px;
    uint32_t pixel_format;   /* maps to kDPU_PixelFormatRGB565 etc. from fsl_dpu.h */
    uint32_t framebuffer_addr; /* physical address of the OCRAM/DDR framebuffer */
} dpu_driver_config_t;

int  dpu_driver_init(const dpu_driver_config_t *config);
int  dpu_driver_start(void);
void dpu_driver_stop(void);

/* frame_id: symbolic buffer selector ("logo" / "safe" / future
 * double-buffer index). Swaps the active plane's source address; does
 * not itself touch MIPI-DSI (that is display_link/mipi_dsi's job). */
void dpu_driver_submit_frame(const char *frame_id);
