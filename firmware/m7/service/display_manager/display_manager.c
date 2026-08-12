#include "display_manager.h"
#include "display_hal.h"

void display_manager_init(void)
{
    display_hal_init();
}

void display_manager_start(void)
{
    display_hal_start();
}

void display_manager_show_logo(void)
{
    display_hal_show_logo();
}

void display_manager_show_safe_display(void)
{
    display_hal_show_safe_display();
}
