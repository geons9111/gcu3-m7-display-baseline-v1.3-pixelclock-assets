#include "mipi_dsi_hal.h"
#include "mipi_dsi_driver.h"

int mipi_dsi_hal_init(const mipi_dsi_hal_config_t *config)
{
    mipi_dsi_driver_config_t drv_cfg = {
        .width_px       = config->width_px,
        .height_px      = config->height_px,
        .lane_count     = config->lane_count,
        .hs_bitrate_bps = config->hs_bitrate_bps,
        .pixel_clock_hz = config->pixel_clock_hz
    };
    return mipi_dsi_driver_init(&drv_cfg);
}

void mipi_dsi_hal_start(void)
{
    (void)mipi_dsi_driver_start();
}
