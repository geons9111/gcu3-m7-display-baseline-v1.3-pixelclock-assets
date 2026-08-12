#pragma once

#include <stdbool.h>

void display_hal_init(void);
void display_hal_start(void);
void display_hal_show_logo(void);
void display_hal_show_safe_display(void);

/* 物理Display Link (adapter/health_check)が起動に失敗した場合、
 * falseを返します。boot_manager/fault_managerはこれを使用して
 * A55ハンドオーバーに進むかSAFE_DISPLAYに留まるかを決定します
 * (baseline v1.1 §9 Ownership state machine, REQ-M7-05). */
bool display_hal_link_healthy(void);
