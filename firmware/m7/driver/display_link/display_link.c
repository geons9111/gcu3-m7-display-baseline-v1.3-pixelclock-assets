#include "display_link.h"
#include "hdmi_adapter.h"
#include "fpd_link_adapter.h"
#include "gmsl_adapter.h"

static display_link_type_t s_backend = DISPLAY_LINK_FPD_LINK;

int display_link_init(void)
{
    switch (s_backend) {
    case DISPLAY_LINK_HDMI:
        return hdmi_adapter_init();
    case DISPLAY_LINK_FPD_LINK:
        return fpd_link_adapter_init();
    case DISPLAY_LINK_GMSL:
        return gmsl_adapter_init();
    default:
        return -1;
    }
}

int display_link_start(void)
{
    switch (s_backend) {
    case DISPLAY_LINK_HDMI:
        return hdmi_adapter_start();
    case DISPLAY_LINK_FPD_LINK:
        return fpd_link_adapter_start();
    case DISPLAY_LINK_GMSL:
        return gmsl_adapter_start();
    default:
        return -1;
    }
}

int display_link_health_check(void)
{
    switch (s_backend) {
    case DISPLAY_LINK_HDMI:
        return 0; /* TODO: HDMI adapter health_check not yet implemented */
    case DISPLAY_LINK_FPD_LINK:
        return fpd_link_adapter_health_check();
    case DISPLAY_LINK_GMSL:
        return 0; /* TODO: GMSL adapter health_check not yet implemented */
    default:
        return -1;
    }
}
