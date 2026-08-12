#include "m7_application.h"
#include "boot_manager.h"
#include "health_manager.h"
#include "ipc_manager.h"
#include "timer_hal.h"

void m7_application_init(void)
{
    /* Initialize HALs not covered by BSP */
    timer_hal_init();

    /* Initialize managers */
    boot_manager_init();
    ipc_manager_init();
    health_manager_init();
}

void m7_application_run(void)
{
    while (1) {
        if (boot_manager_state() != BOOT_STATE_BOOT_COMPLETE) {
            boot_manager_run();
        }

        ipc_manager_poll();
        health_manager_poll();
    }
}
