#include "display_hal.h"
#include "display_engine.h"
#include "display_link.h"

void display_hal_init(void)
{
    display_engine_init();
    (void)display_link_init();
}

void display_hal_start(void)
{
    display_engine_start();
    (void)display_link_start();
}

void display_hal_show_logo(void)
{
    display_engine_render_logo();
}

void display_hal_show_safe_display(void)
{
    display_engine_render_safe();
}

bool display_hal_link_healthy(void)
{
    return display_link_health_check() == 0;
}
