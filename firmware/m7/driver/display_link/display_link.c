#include "display_link.h"
#include "hdmi_adapter.h"
#include "fpd_link_adapter.h"
#include "gmsl_adapter.h"

/* Build-time selection (baseline v1.1 §21 / ARCH-009): Display Link is
 * NOT auto-detected at runtime. Default here is FPD-Link, matching the
 * vehicle production path this task targets (DS90UB941/948). */
#ifndef DISPLAY_LINK_BACKEND
#define DISPLAY_LINK_BACKEND 1
#endif

int display_link_init(void)
{
#if DISPLAY_LINK_BACKEND == 0
    return hdmi_adapter_init();
#elif DISPLAY_LINK_BACKEND == 1
    return fpd_link_adapter_init();
#else
    return gmsl_adapter_init();
#endif
}

int display_link_start(void)
{
#if DISPLAY_LINK_BACKEND == 0
    return hdmi_adapter_start();
#elif DISPLAY_LINK_BACKEND == 1
    return fpd_link_adapter_start();
#else
    return gmsl_adapter_start();
#endif
}

int display_link_health_check(void)
{
#if DISPLAY_LINK_BACKEND == 0
    return 0; /* TODO: HDMI adapter health_check not yet implemented */
#elif DISPLAY_LINK_BACKEND == 1
    return fpd_link_adapter_health_check();
#else
    return 0; /* TODO: GMSL adapter health_check not yet implemented */
#endif
}
