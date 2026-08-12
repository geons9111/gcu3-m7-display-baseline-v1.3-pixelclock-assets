#pragma once
/*
 * i2c_hal.h
 *
 * GCU3 M7 — I2C HAL (LPI2C wrapper)
 *
 * Isolates all raw register access to fsl_lpi2c.h (MCUXpresso SDK) below
 * this HAL, per ARCH-005 (Driver Isolation). Adapter/driver code (e.g.
 * fpd_link_adapter, gmsl_adapter) must never call LPI2C_* directly.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum
{
    I2C_HAL_OK = 0,
    I2C_HAL_ERROR_TIMEOUT,
    I2C_HAL_ERROR_NACK,
    I2C_HAL_ERROR_BUS
} i2c_hal_status_t;

/*
 * i2c_hal_init
 *   instance    : LPI2C instance index used for the DSI-to-SerDes bridge
 *                 (board-specific; confirm against imx95-evk schematic
 *                 before production use — placeholder default is
 *                 LPI2C instance 2, matching the display header on the
 *                 imx95-19x19-lpddr5-evk).
 *   baudrate_hz : bus speed. Standard-mode 100kHz for bring-up; most
 *                 SerDes bridges (DS90UBxx / MAX96xxx) support Fast-mode
 *                 400kHz once link-locked, per REQ-M7-03 in the earlier
 *                 fast-boot review (kept as fast_boot_mode flag here).
 */
i2c_hal_status_t i2c_hal_init(uint32_t instance, uint32_t baudrate_hz);
void             i2c_hal_deinit(void);

i2c_hal_status_t i2c_hal_write_reg8(uint8_t device_addr, uint8_t reg, uint8_t value);
i2c_hal_status_t i2c_hal_read_reg8(uint8_t device_addr, uint8_t reg, uint8_t *value);

/* Batch write for bridge init sequences (REQ-M7-03: batch transfer to
 * minimize bring-up latency vs. one transaction per register). */
typedef struct
{
    uint8_t reg;
    uint8_t value;
} i2c_hal_reg_write_t;

i2c_hal_status_t i2c_hal_write_sequence(uint8_t device_addr,
                                         const i2c_hal_reg_write_t *sequence,
                                         size_t count);
