/*
 * fpd_link_adapter.c
 *
 * GCU3 M7 — DS90UB941AS-Q1 (serializer, DSI-to-SerDes bridge, "SDSB")
 * initialization over I2C, per baseline v1.1 §16 Production Path and
 * the earlier fast-boot review's REQ-M7-03/REQ-M7-05.
 *
 * IMPORTANT — register map disclaimer:
 *   The register addresses/values below follow the DS90UB941/948
 *   family's publicly documented register layout (general
 *   configuration, DSI port config, PLL/clock, link-lock status), but
 *   exact bit-field values MUST be re-verified against the specific
 *   DS90UB941AS-Q1 datasheet revision and the actual board's strap/
 *   GPIO wiring before production use. Do not flash to a vehicle ECU
 *   without that verification — this is a bring-up skeleton, not a
 *   certified register sequence.
 */

#include "fpd_link_adapter.h"
#include "i2c_hal.h"

#include <stdint.h>
#include <stdbool.h>

/* 7-bit I2C address as seen by M7; DS90UB941 default I2C address,
 * confirm against board address-select strap (ADDR_ID pin). */
#define DS90UB941_I2C_ADDR   0x0CU

/* Register offsets — TBD: verify against DS90UB941AS-Q1 datasheet. */
#define REG_GENERAL_CFG      0x03U
#define REG_DSI_LANE_CFG     0x1CU
#define REG_PLL_CTRL         0x0DU
#define REG_LINK_STATUS      0x0AU
#define BIT_LINK_LOCKED      (1U << 0)

#define FPD_LINK_I2C_INSTANCE   2U       /* TBD: confirm LPI2C instance vs. schematic */
#define FPD_LINK_I2C_BAUD_HZ    400000U  /* Fast-mode, REQ-M7-03 latency optimization */
#define FPD_LINK_LOCK_TIMEOUT_POLLS  50U /* REQ-M7-05: bounded retry, not infinite poll */

static bool s_initialized = false;

int fpd_link_adapter_init(void)
{
    if (i2c_hal_init(FPD_LINK_I2C_INSTANCE, FPD_LINK_I2C_BAUD_HZ) != I2C_HAL_OK)
    {
        return -1;
    }

    /* Batch init sequence (REQ-M7-03): single transaction burst rather
     * than per-register round trips, to minimize link bring-up latency
     * within the 1.0s logo budget (baseline v1.1 §7). */
    static const i2c_hal_reg_write_t init_seq[] = {
        {REG_GENERAL_CFG,  0x9CU}, /* TBD: enable DSI input, i2c pass-through off */
        {REG_DSI_LANE_CFG, 0x04U}, /* TBD: 4-lane DSI, matches GCU3_DSI_LANE_COUNT */
        {REG_PLL_CTRL,     0x01U}, /* TBD: enable PLL, select ref clock source */
    };

    i2c_hal_status_t st = i2c_hal_write_sequence(DS90UB941_I2C_ADDR, init_seq,
                                                  sizeof(init_seq) / sizeof(init_seq[0]));
    if (st != I2C_HAL_OK)
    {
        return -1;
    }

    s_initialized = true;
    return 0;
}

int fpd_link_adapter_start(void)
{
    if (!s_initialized)
    {
        return -1;
    }

    /* REQ-M7-05 fail-safe: bounded poll for link lock, not indefinite
     * blocking. On timeout, return failure so display_link.c can decide
     * whether to retry once (per REQ-M7-05) or escalate to fault_manager. */
    for (uint32_t i = 0; i < FPD_LINK_LOCK_TIMEOUT_POLLS; i++)
    {
        if (fpd_link_adapter_health_check() == 0)
        {
            return 0;
        }
        /* Deliberately no blocking delay call here — caller (boot_manager
         * state machine) owns timing/scheduling; this driver stays
         * non-blocking to avoid stalling other boot-critical-path work. */
    }

    return -1; /* link not locked within budget */
}

int fpd_link_adapter_stop(void)
{
    static const i2c_hal_reg_write_t stop_seq[] = {
        {REG_PLL_CTRL, 0x00U}, /* TBD: disable PLL */
    };
    i2c_hal_status_t st = i2c_hal_write_sequence(DS90UB941_I2C_ADDR, stop_seq, 1U);
    s_initialized = false;
    return (st == I2C_HAL_OK) ? 0 : -1;
}

int fpd_link_adapter_health_check(void)
{
    uint8_t status = 0U;
    if (i2c_hal_read_reg8(DS90UB941_I2C_ADDR, REG_LINK_STATUS, &status) != I2C_HAL_OK)
    {
        return -1;
    }
    return ((status & BIT_LINK_LOCKED) != 0U) ? 0 : -1;
}
