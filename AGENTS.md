# AGENTS.md - Project Guidelines & Rules

## 1. Language Policy (言語・文書作成ポリシー)
- **User Chat**: Answer in **Korean (한국어)**.
- **AI Architecture Documents (`ai/`)**: Written in **Japanese (日本語)**.
- **User Documents (`docs/`, `README.md`)**: Written in **Japanese (日本語)**.
- **Code Comments & TODOs (`.c`, `.h`, `.cmake`)**: Written in **Japanese (日本語)**.

## 2. 2-Stage Development Strategy (2段階開発戦略)
- **Phase 1 (現在: HW Bring-up)**: NXP SDKサンプルコードを直接活用し、DPU, MIPI-DSI, FPD-Link HW動作確認および画面出力を迅速に達成する。
- **Phase 2 (HW検証完了後: AUTOSAR-Ready転換)**: 動作確認済みドライバーの上位に`Std_Types`および`CDD`インターフェースラッパーを実装し、AUTOSAR-Ready構造へ段階的に転換する。

## 3. MVC (Model-View-Controller) Architectural Principle (MVC設計方針)
- **Model**: 車両データおよび状態フラグ（`health_manager`, `fault_manager`, `ownership_manager`）のみを管理し、描画処理を持たない。
- **View**: `display_engine` / `dpu_driver` は描画判断ロジックを持たず、Controllerから指定されたフレームバッファの高速スキャンアウトに専念する。
- **Controller**: `m7_application` / `boot_manager` が Model の状態変化を監視し、View の画面切り替え・A核ハンドオーバーを統括する。
