# GCU3 M7 Display Implementation Blueprint v1.2

## Implementation sequence

1. Phase A Gate closure
2. HAL contract freeze
3. Driver contract freeze
4. Display Engine skeleton
5. Display Manager
6. Ownership Manager
7. IPC Manager
8. Fault Manager
9. QEMU contract tests
10. NXP hardware adapter implementation
11. A55 handover integration
12. Vehicle monitor validation

## Current code status

The `firmware/m7` source tree is an architectural skeleton. Hardware-specific register access, NXP adapter register tables, exact MIPI-DSI timing, monitor resolution/pixel clock, and production IPC transport are intentionally stubbed until the corresponding architecture gates are verified.

## TODO: 実務開発戦略 (2段階アプローチ)

- **Phase 1 (現在の開発フェーズ - HW Bring-up優先)**:
  - NXP MCUXpresso SDKサンプルコード (`fsl_dpu.h`, `fsl_mipi_dsi.h`, `fsl_lpi2c.h`) を直接活用する。
  - DPU, MIPI-DSI, FPD-Link ハードウェアの動作検証および画面出力を最優先で達成する。
- **Phase 2 (HW動作検証完了後 - AUTOSAR-Ready転換)**:
  - 動作確認済みのドライバーコードをベースに、`Std_Types.h` および `CDD` (Complex Device Driver) ラッパーを実装する。
  - Service層モジュールを `Runnable` 構造に再構築し、AUTOSAR-Ready構造へ移行する。

## MVC (Model-View-Controller) アーキテクチャ構成
- **Model**: 車両データ / 診断状態 (`diagnostic_manager`, `fault_manager`, `health_manager`, `ownership_manager`)
- **View**: 表示パイプライン / フレームバッファ出力 (`display_engine`, `dpu_driver`, `mipi_dsi_hal`, `framebuffer_assets`)
- **Controller**: 起動・遷移オーケストレーション (`m7_application`, `boot_manager`, `display_manager`)

## Important

The current values in `display_config.h` marked `provisional` are placeholders and are not production calibration values.
