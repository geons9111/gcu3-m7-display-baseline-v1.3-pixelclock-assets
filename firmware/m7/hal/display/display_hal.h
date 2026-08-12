#pragma once

#include <stdbool.h>

void display_hal_init(void);
void display_hal_start(void);
void display_hal_show_logo(void);
void display_hal_show_safe_display(void);

/* Returns false if the physical Display Link (adapter/health_check)
 * failed to come up — boot_manager/fault_manager use this to decide
 * whether to proceed to A55 handover or stay in SAFE_DISPLAY
 * (baseline v1.1 §9 Ownership state machine, REQ-M7-05). */
bool display_hal_link_healthy(void);
