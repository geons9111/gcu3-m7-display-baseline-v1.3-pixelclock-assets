#pragma once
#include <stdint.h>

typedef struct
{
    uint16_t width_px;
    uint16_t height_px;
    uint8_t  lane_count;
    uint32_t hs_bitrate_bps;
    uint32_t pixel_clock_hz;
} mipi_dsi_hal_config_t;

int  mipi_dsi_hal_init(const mipi_dsi_hal_config_t *config);
void mipi_dsi_hal_start(void);
