#pragma once

typedef enum {
    BOOT_STATE_RESET = 0,
    BOOT_STATE_HW_INIT,
    BOOT_STATE_DISPLAY_INIT,
    BOOT_STATE_LOGO,
    BOOT_STATE_PASSWORD_WAIT,
    BOOT_STATE_BOOT_COMPLETE
} boot_state_t;

void boot_manager_init(void);
void boot_manager_run(void);
boot_state_t boot_manager_state(void);
