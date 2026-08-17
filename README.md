# GCU3 M7 Display Software Baseline v1.2

## Architecture

`Application → Service → HAL / Display Engine → Driver → BSP / Platform`

### MVC (Model-View-Controller) 設計パターン
本ファームウェアは、高速起動と表示制御の安全性を担保するため **MVCパターン** に基づいてモジュールが明確に分離されています。
- **Model (データ・状態管理)**: `health_manager`, `fault_manager`, `ownership_manager`, CANテレメトリデータ
- **View (表示・スキャンアウト)**: `framebuffer_assets`, `display_engine`, `dpu_driver`, `mipi_dsi_hal`
- **Controller (シーケンス制御・調停)**: `m7_application`, `boot_manager`, `display_manager`

## Implementation

The repository now contains the first compilable architectural skeleton under `firmware/m7`.

### Production path
- `firmware/m7/app`
- `firmware/m7/service`
- `firmware/m7/hal`
- `firmware/m7/display_engine`
- `firmware/m7/driver`
- `firmware/m7/bsp`
- `firmware/m7/platform`

### Verification path
- `tests/unit`
- `tests/integration`
- `tests/fault_injection`
- `tests/qemu`

### Design authority
- `ai/architecture/baseline/GCU3-M7-Display-Software-Baseline-v1.2.md`
- `ai/architecture/baseline/IMPLEMENTATION-BLUEPRINT-v1.2.md`

## Important implementation status

Hardware-specific implementation remains gated:

- GATE-01 IPC/LMM feasibility: OPEN
- GATE-05 Secure Boot/AHAB scope: OPEN
- GATE-02 heartbeat thresholds: OPEN
- GATE-03 DPU driver boundary: DEFINED
- GATE-04 diagnostic interface: DEFINED

Therefore the source code is a **software architecture skeleton**, not yet a production-qualified driver implementation.
