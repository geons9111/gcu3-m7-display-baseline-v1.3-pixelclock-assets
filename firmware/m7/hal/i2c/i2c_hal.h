#pragma once
/*
 * i2c_hal.h
 *
 * GCU3 M7 — I2C HAL (LPI2Cラッパー)
 *
 * ARCH-005 (Driver Isolation)に従い、fsl_lpi2c.h (MCUXpresso SDK) へのすべての生レジスタアクセスを
 * このHALの下に分離します。アダプタ/ドライバコード (例: fpd_link_adapter, gmsl_adapter) は
 * 絶対に LPI2C_* を直接呼び出してはいけません。
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
 *   instance    : DSI-to-SerDesブリッジに使用されるLPI2Cインスタンスインデックス
 *                 (ボード固有。本番環境で使用する前に imx95-evk の回路図で確認してください。
 *                 プレースホルダーのデフォルトは、imx95-19x19-lpddr5-evk のディスプレイヘッダーと一致する
 *                 LPI2C インスタンス 2 です)。
 *   baudrate_hz : バス速度。立ち上げ時はStandard-modeの100kHz。ほとんどの
 *                 SerDesブリッジ(DS90UBxx / MAX96xxx)は、以前のfast-bootレビューのREQ-M7-03に従い、
 *                 リンクロックされるとFast-mode 400kHzをサポートします（ここでは fast_boot_mode フラグとして保持）。
 */
i2c_hal_status_t i2c_hal_init(uint32_t instance, uint32_t baudrate_hz);
void             i2c_hal_deinit(void);

i2c_hal_status_t i2c_hal_write_reg8(uint8_t device_addr, uint8_t reg, uint8_t value);
i2c_hal_status_t i2c_hal_read_reg8(uint8_t device_addr, uint8_t reg, uint8_t *value);

/* ブリッジ初期化シーケンスの一括書き込み（REQ-M7-03: 1レジスタごとのトランザクションと比較して
 * 立ち上げのレイテンシを最小限に抑えるための一括転送）。 */
typedef struct
{
    uint8_t reg;
    uint8_t value;
} i2c_hal_reg_write_t;

i2c_hal_status_t i2c_hal_write_sequence(uint8_t device_addr,
                                         const i2c_hal_reg_write_t *sequence,
                                         size_t count);
