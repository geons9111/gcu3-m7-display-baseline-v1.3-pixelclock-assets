#include "boot_manager.h"
#include "display_manager.h"
#include "ownership_manager.h"

static boot_state_t state = BOOT_STATE_RESET;

void boot_manager_init(void)
{
    state = BOOT_STATE_HW_INIT;
}

void boot_manager_run(void)
{
    switch (state) {
    case BOOT_STATE_HW_INIT:
        display_manager_init();
        ownership_manager_set_m7_owner();
        state = BOOT_STATE_DISPLAY_INIT;
        break;

    case BOOT_STATE_DISPLAY_INIT:
        display_manager_start();
        state = BOOT_STATE_LOGO;
        break;

    case BOOT_STATE_LOGO:
        display_manager_show_logo();
        state = BOOT_STATE_PASSWORD_WAIT;
        break;

    case BOOT_STATE_PASSWORD_WAIT:
        /* User interaction is intentionally outside the boot-time budget. */
        state = BOOT_STATE_BOOT_COMPLETE;
        break;

    case BOOT_STATE_BOOT_COMPLETE:
        ownership_manager_request_a55_handover();
        break;

    default:
        break;
    }
}

boot_state_t boot_manager_state(void)
{
    return state;
}
