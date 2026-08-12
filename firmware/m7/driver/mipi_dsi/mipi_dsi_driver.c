/*
 * mipi_dsi_driver.c
 *
 * GCU3 M7 — MIPI-DSI host driver over MCUXpresso SDK fsl_mipi_dsi.h.
 *
 * i.MX95 DSI host is a Synopsys DesignWare DSI controller (same IP
 * family as i.MX8M/93); the SDK exposes it via MIPI_DSI_Init /
 * dsi_dpi_config_t / dsi_config_t. Base address and clock names below
 * follow the SDK's fsl_device_registers.h for MIMX95 — verify against
 * the actual mcux-sdk checkout before first real-hardware bring-up
 * (see GCU3 TODO: LPUART_BASE-style placeholder caveat applies equally
 * here to MIPI_DSI base/clock names).
 */

#include "mipi_dsi_driver.h"

#include "fsl_mipi_dsi.h"
#include "fsl_clock.h"

static MIPI_DSI_HOST_Type *const s_dsi_base = MIPI_DSI0; /* TBD: confirm instance vs. board routing */

static dsi_config_t          s_dsi_config;
static dsi_dphy_config_t     s_dphy_config;
static mipi_dsi_driver_config_t s_cfg;

int mipi_dsi_driver_init(const mipi_dsi_driver_config_t *config)
{
    if (config == NULL || config->lane_count == 0U || config->lane_count > 4U)
    {
        return -1;
    }
    s_cfg = *config;

    DSI_GetDefaultConfig(&s_dsi_config);
    s_dsi_config.numLanes   = s_cfg.lane_count;
    s_dsi_config.enableHsClk = true; /* continuous HS clock: reduces per-frame
                                         DPHY lock latency, favors fast-boot
                                         over the marginal power cost -
                                         acceptable per REQ-M7-02 minimal-init. */

    /* DPHY PLL: derive from requested per-lane HS bitrate. The SDK
     * helper computes PLL divider settings; refClkFreq is the DSI
     * reference clock as configured by imx95_bsp_init(). */
    uint32_t dsi_ref_clk_hz = CLOCK_GetIpFreq(kCLOCK_Root_MipiDsiRef);
    DSI_GetDphyDefaultConfig(&s_dphy_config, s_cfg.hs_bitrate_bps, dsi_ref_clk_hz);

    status_t st = DSI_Init(s_dsi_base, &s_dsi_config);
    if (st != kStatus_Success)
    {
        return -1;
    }

    st = DSI_InitDphy(s_dsi_base, &s_dphy_config, dsi_ref_clk_hz);
    if (st != kStatus_Success)
    {
        return -1;
    }

    return 0;
}

int mipi_dsi_driver_start(void)
{
    /* Enter HS mode for pixel data transmission. Command mode (LP) is
     * used only for the bring-up DCS handshake in
     * mipi_dsi_driver_dcs_write(); the bulk of runtime traffic is video
     * mode over the DPI->DSI datapath configured by the DPU driver. */
    DSI_SetDpiConfig(s_dsi_base, &s_dsi_config, s_cfg.lane_count,
                      s_cfg.width_px, s_cfg.height_px);
    return 0;
}

void mipi_dsi_driver_stop(void)
{
    DSI_Deinit(s_dsi_base);
}

int mipi_dsi_driver_dcs_write(uint8_t dcs_cmd, const uint8_t *data, uint16_t length)
{
    dsi_transfer_t xfer = {0};
    xfer.txDataSize = length;
    xfer.txData     = data;
    xfer.sendCmd    = dcs_cmd;
    xfer.subLpm     = false;

    status_t st = (length <= 2U)
                      ? DSI_TransferWrite(s_dsi_base, &xfer)   /* short DCS write */
                      : DSI_TransferWrite(s_dsi_base, &xfer);  /* long DCS write; SDK
                                                                   picks the correct
                                                                   packet type based
                                                                   on txDataSize. */
    return (st == kStatus_Success) ? 0 : -1;
}
