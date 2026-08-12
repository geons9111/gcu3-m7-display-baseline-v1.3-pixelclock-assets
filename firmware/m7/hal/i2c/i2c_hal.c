/*
 * i2c_hal.c
 *
 * GCU3 M7 — MCUXpresso SDK LPI2Cドライバ上でのI2C HAL実装。
 *
 * 注: fsl_lpi2c.h / fsl_clock.h は firmware/m7/CMakeLists.txt の SdkRootDirPath によって参照される
 * i.MX95 MCUXpresso SDK (mcuxsdk) から提供されます。
 * このファイルは、M7 hello_world 立ち上げファームウェアで使用される既存の GCU3 の hardware_init
 * パターンに従い、SDK の LPI2C マスタードライバ (fsl_lpi2c.h) とボードの LPI2C クロックルートが
 * imx95_bsp_init() (pinmux + clock gating) によってすでに立ち上げられていることを前提とします。
 */

#include "i2c_hal.h"

#include "fsl_lpi2c.h"
#include "fsl_clock.h"

static LPI2C_Type *s_i2c_base   = NULL;
static uint32_t     s_i2c_srcclk = 0U;

/* imx95lpd5evk19 LPI2C インスタンスのベーステーブル。
 * 本番環境での使用前に、ボード回路図 / fsl_device_registers.h を確認してください。
 * 未確認のアドレスに関する以前のレビューの指摘に従い、(暗黙的に推測せず)明示的に保持しています。 */
static LPI2C_Type *i2c_hal_instance_base(uint32_t instance)
{
    switch (instance)
    {
        case 0: return LPI2C1;
        case 1: return LPI2C2;
        case 2: return LPI2C3;
        case 3: return LPI2C4;
        default: return NULL;
    }
}

i2c_hal_status_t i2c_hal_init(uint32_t instance, uint32_t baudrate_hz)
{
    s_i2c_base = i2c_hal_instance_base(instance);
    if (s_i2c_base == NULL)
    {
        return I2C_HAL_ERROR_BUS;
    }

    lpi2c_master_config_t config;
    LPI2C_MasterGetDefaultConfig(&config);
    config.baudRate_Hz = baudrate_hz;

    /* Source clock: 実際のCCMルートの選択はBSPが行います。このHALは
     * 結果の周波数を読み戻すだけです。 */
    s_i2c_srcclk = CLOCK_GetIpFreq(kCLOCK_Root_Lpi2c1 + instance);

    LPI2C_MasterInit(s_i2c_base, &config, s_i2c_srcclk);
    return I2C_HAL_OK;
}

void i2c_hal_deinit(void)
{
    if (s_i2c_base != NULL)
    {
        LPI2C_MasterDeinit(s_i2c_base);
        s_i2c_base = NULL;
    }
}

i2c_hal_status_t i2c_hal_write_reg8(uint8_t device_addr, uint8_t reg, uint8_t value)
{
    if (s_i2c_base == NULL)
    {
        return I2C_HAL_ERROR_BUS;
    }

    uint8_t buf[2] = {reg, value};
    status_t st = LPI2C_MasterStart(s_i2c_base, device_addr, kLPI2C_Write);
    if (st != kStatus_Success)
    {
        return I2C_HAL_ERROR_NACK;
    }

    st = LPI2C_MasterSend(s_i2c_base, buf, sizeof(buf));
    LPI2C_MasterStop(s_i2c_base);

    return (st == kStatus_Success) ? I2C_HAL_OK : I2C_HAL_ERROR_NACK;
}

i2c_hal_status_t i2c_hal_read_reg8(uint8_t device_addr, uint8_t reg, uint8_t *value)
{
    if (s_i2c_base == NULL || value == NULL)
    {
        return I2C_HAL_ERROR_BUS;
    }

    status_t st = LPI2C_MasterStart(s_i2c_base, device_addr, kLPI2C_Write);
    if (st != kStatus_Success)
    {
        return I2C_HAL_ERROR_NACK;
    }
    st = LPI2C_MasterSend(s_i2c_base, &reg, 1U);
    if (st != kStatus_Success)
    {
        LPI2C_MasterStop(s_i2c_base);
        return I2C_HAL_ERROR_NACK;
    }

    st = LPI2C_MasterRepeatedStart(s_i2c_base, device_addr, kLPI2C_Read);
    if (st != kStatus_Success)
    {
        LPI2C_MasterStop(s_i2c_base);
        return I2C_HAL_ERROR_NACK;
    }
    st = LPI2C_MasterReceive(s_i2c_base, value, 1U);
    LPI2C_MasterStop(s_i2c_base);

    return (st == kStatus_Success) ? I2C_HAL_OK : I2C_HAL_ERROR_NACK;
}

i2c_hal_status_t i2c_hal_write_sequence(uint8_t device_addr,
                                         const i2c_hal_reg_write_t *sequence,
                                         size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        i2c_hal_status_t st = i2c_hal_write_reg8(device_addr, sequence[i].reg, sequence[i].value);
        if (st != I2C_HAL_OK)
        {
            return st;
        }
    }
    return I2C_HAL_OK;
}
