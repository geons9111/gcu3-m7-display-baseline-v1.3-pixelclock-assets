/*
 * dpu_driver.c
 *
 * GCU3 M7 — DPU driver over MCUXpresso SDK fsl_dpu.h.
 *
 * i.MX95 DPU composes one or more framebuffer planes and feeds the
 * result to the MIPI-DSI host over the internal DPI interface. This
 * driver configures a single fullscreen plane (no blending) sufficient
 * for logo / safe-display / (later) runtime handover use cases, per
 * baseline v1.1 §12 (DPU as a fetch/compose engine feeding DSI, not a
 * general compositor at this stage).
 */

#include "dpu_driver.h"

#include <string.h>
#include <stdint.h>

#include "fsl_dpu.h"

static DPU_Type *const s_dpu_base = DPU0; /* TBD: confirm instance for imx95lpd5evk19 */

static dpu_display_timing_t s_timing;
static dpu_driver_config_t  s_cfg;

/* Static logo/safe framebuffer addresses. Both live in TCM/OCRAM per
 * REQ-M7-04 (no runtime file I/O, no decode step, embedded at build
 * time) — actual symbol comes from the generated framebuffer asset
 * (see display_engine/framebuffer/, not yet populated by this change).
 */
extern const uint8_t g_gcu3_logo_framebuffer[];
extern const uint8_t g_gcu3_safe_framebuffer[];

int dpu_driver_init(const dpu_driver_config_t *config)
{
    if (config == NULL)
    {
        return -1;
    }
    s_cfg = *config;

    dpu_config_t dpu_init_config;
    DPU_GetDefaultConfig(&dpu_init_config);
    DPU_Init(s_dpu_base, &dpu_init_config);

    DPU_GetDefaultDisplayTimingConfig(&s_timing);
    s_timing.width  = s_cfg.width_px;
    s_timing.height = s_cfg.height_px;
    /* GCU3_DISPLAY_PIXEL_CLOCK_HZ is currently 0 (TBD in
     * config/display_config.h pending vehicle monitor datasheet) — do
     * not silently compute a guessed value here; require the caller
     * (display_engine) to fail closed if pixel_clock_hz is unset. */

    dpu_fetchunit_config_t fetch_config = {
        .width       = s_cfg.width_px,
        .height      = s_cfg.height_px,
        .format      = (dpu_pixel_format_t)s_cfg.pixel_format,
        .baseAddr    = s_cfg.framebuffer_addr,
        .stride      = (uint32_t)s_cfg.width_px * 2U, /* RGB565 default; revisit
                                                           if pixel_format changes */
    };

    DPU_SetFetchUnitConfig(s_dpu_base, kDPU_FetchUnit0, &fetch_config);
    DPU_SetDisplayTimingConfig(s_dpu_base, kDPU_Display0, &s_timing);

    return 0;
}

int dpu_driver_start(void)
{
    DPU_EnableDisplay(s_dpu_base, kDPU_Display0, true);
    return 0;
}

void dpu_driver_stop(void)
{
    DPU_EnableDisplay(s_dpu_base, kDPU_Display0, false);
}

void dpu_driver_submit_frame(const char *frame_id)
{
    const uint8_t *fb = NULL;

    if (frame_id == NULL)
    {
        return;
    }
    if (strcmp(frame_id, "logo") == 0)
    {
        fb = g_gcu3_logo_framebuffer;
    }
    else if (strcmp(frame_id, "safe") == 0)
    {
        fb = g_gcu3_safe_framebuffer;
    }
    else
    {
        return; /* unknown frame id: fail closed, keep previous plane */
    }

    DPU_SetFetchUnitBaseAddr(s_dpu_base, kDPU_FetchUnit0, (uint32_t)(uintptr_t)fb);
    DPU_TriggerShadowLoad(s_dpu_base, kDPU_Display0);
}
