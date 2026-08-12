#pragma once
/*
 * flexcan_driver.h
 *
 * GCU3 M7 — FlexCAN ドライバー
 * レジスタへのアクセスを分離するために MCUXpresso SDK の fsl_flexcan.h をラップします。
 */

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t id;          /* CAN ID (標準 11-bit または 拡張 29-bit) */
    uint8_t  data[8];     /* クラシック CAN ペイロード */
    uint8_t  dlc;         /* データ長コード */
    bool     is_extended; /* 拡張フレームフラグ */
} flexcan_frame_t;

typedef struct {
    uint32_t instance;     /* FlexCAN インスタンス (0 = CAN1, 1 = CAN2, ...) */
    uint32_t baudrate_bps; /* 例: 500000U (500 kbps) */
    uint32_t rx_mb_count;  /* 受信メッセージバッファの数 */
} flexcan_driver_config_t;

int  flexcan_driver_init(const flexcan_driver_config_t *config);
int  flexcan_driver_receive(flexcan_frame_t *frame);
void flexcan_driver_deinit(void);
