#pragma once
/*
 * mipi_dsi_driver.h
 *
 * GCU3 M7 — MIPI-DSI ホストコントローラードライバー (ARCH-005: レジスタへのアクセスは
 * このファイルに分離されます。hal/mipi_dsi/mipi_dsi_hal.c が唯一の呼び出し元です)。
 *
 * mcux-sdk で i.MX95 用に提供されている MCUXpresso SDK Synopsys DesignWare DSI
 * ホストドライバー (fsl_mipi_dsi.h) をラップします。
 */

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint16_t width_px;
    uint16_t height_px;
    uint8_t  lane_count;      /* DSI データレーン数 (ボード: パネル/ブリッジのデータシートと照らし合わせて確認すること) */
    uint32_t hs_bitrate_bps;  /* レーンごとの HS ビットレート。DPHY PLL の設定を駆動します */
    uint32_t pixel_clock_hz;  /* display_config.h からの GCU3_DISPLAY_PIXEL_CLOCK_HZ */
} mipi_dsi_driver_config_t;

int  mipi_dsi_driver_init(const mipi_dsi_driver_config_t *config);
int  mipi_dsi_driver_start(void);
void mipi_dsi_driver_stop(void);

/* 汎用 DSI ショート/ロングコマンドパススルー。ブリッジの I2C 制御ポートではなく、
 * DSI リンク自体を通過する必要があるブリッジ側の DCS コマンドのために、
 * DSI-to-SerDes アダプタレイヤー (fpd_link_adapter) によって使用されます。 */
int mipi_dsi_driver_dcs_write(uint8_t dcs_cmd, const uint8_t *data, uint16_t length);
