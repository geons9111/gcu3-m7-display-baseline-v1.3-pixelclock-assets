#pragma once
/*
 * fpd_link_adapter.h
 *
 * GCU3 M7 — DSI-to-FPD-Link III SerDes ブリッジアダプタ
 * (シリアライザ: DS90UB941AS-Q1, デシリアライザ: DS90UB948-Q1 — ベースライン
 * v1.1 §16 運用パス)。
 *
 * ARCH-005: すべての DS90UBxx レジスタアクセスはこの .c ファイルに分離され、
 * hal/i2c/i2c_hal.h を介してのみアクセスされます。display_link.c
 * (共通の Link_Init/Link_Start/... インターフェース) が唯一の呼び出し元です。
 */

int fpd_link_adapter_init(void);
int fpd_link_adapter_start(void);
int fpd_link_adapter_stop(void);

/* DS90UB941 の LOCK ステータスビットをポーリングします。REQ-M7-05 (フェールセーフ) をサポートします:
 * これが true を返さない場合、呼び出し元 (display_link.c の health_check) は
 * 永遠にブロックするのではなく、タイムアウトしなければなりません。 */
int fpd_link_adapter_health_check(void);
