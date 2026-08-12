#pragma once
/*
 * fpd_link_adapter.h
 *
 * GCU3 M7 — DSI-to-FPD-Link III SerDes bridge adapter
 * (Serializer: DS90UB941AS-Q1, Deserializer: DS90UB948-Q1 — baseline
 * v1.1 §16 Production Path).
 *
 * ARCH-005: all DS90UBxx register access is isolated to this .c file,
 * reached only through hal/i2c/i2c_hal.h. display_link.c (the common
 * Link_Init/Link_Start/... interface) is the only caller.
 */

int fpd_link_adapter_init(void);
int fpd_link_adapter_start(void);
int fpd_link_adapter_stop(void);

/* Polls DS90UB941 LOCK status bit. Backs REQ-M7-05 (fail-safe): caller
 * (display_link.c health_check) must timeout rather than block forever
 * if this never returns true. */
int fpd_link_adapter_health_check(void);
