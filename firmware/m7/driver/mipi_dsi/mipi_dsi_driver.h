#pragma once
/*
 * mipi_dsi_driver.h
 *
 * GCU3 M7 — MIPI-DSI host controller driver (ARCH-005: register access
 * isolated to this file; hal/mipi_dsi/mipi_dsi_hal.c is the only caller).
 *
 * Wraps the MCUXpresso SDK Synopsys DesignWare DSI host driver
 * (fsl_mipi_dsi.h) as provided for i.MX95 in mcux-sdk.
 */

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint16_t width_px;
    uint16_t height_px;
    uint8_t  lane_count;      /* DSI data lane count (board: confirm vs. panel/bridge datasheet) */
    uint32_t hs_bitrate_bps;  /* per-lane HS bitrate; drives DPHY PLL config */
    uint32_t pixel_clock_hz;  /* GCU3_DISPLAY_PIXEL_CLOCK_HZ from display_config.h */
} mipi_dsi_driver_config_t;

int  mipi_dsi_driver_init(const mipi_dsi_driver_config_t *config);
int  mipi_dsi_driver_start(void);
void mipi_dsi_driver_stop(void);

/* Generic DSI short/long command passthrough, used by the DSI-to-SerDes
 * adapter layer (fpd_link_adapter) for bridge-side DCS commands that must
 * traverse the DSI link itself rather than the bridge's I2C control port. */
int mipi_dsi_driver_dcs_write(uint8_t dcs_cmd, const uint8_t *data, uint16_t length);
