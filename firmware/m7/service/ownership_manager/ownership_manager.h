#pragma once

typedef enum {
    DISPLAY_OWNER_M7 = 0,
    DISPLAY_OWNER_HANDOVER,
    DISPLAY_OWNER_A55,
    DISPLAY_OWNER_SAFE
} display_owner_t;

void ownership_manager_set_m7_owner(void);
void ownership_manager_request_a55_handover(void);
void ownership_manager_on_a55_fault(void);
display_owner_t ownership_manager_owner(void);
