# GCU3 M7 Display Software Baseline Architecture v1.2

## 1. Purpose

GCU3 M7을 차량용 Display Fast-Boot 및 Display Safety/Ownership 제어 MCU로 사용하기 위한 소프트웨어 기본 아키텍처 Baseline이다.

## 2. Fixed Architecture Principles

- Application은 Hardware Register를 직접 접근하지 않는다.
- Service Layer는 HAL/Display Engine을 통해 Hardware 기능을 사용한다.
- DPU Register Access는 `driver/dpu/`가 담당한다.
- Display Link는 공통 Interface Contract로 추상화한다.
- HDMI / FPD-Link / GMSL은 동일한 Display Link Interface를 구현할 수 있다.
- QEMU/Mock은 Production Interface Contract를 공유하는 Verification Platform이다.
- Production / Development Configuration은 Build-time으로 분리한다.
- M7은 Fast Boot, deterministic control, fault/safe display를 담당한다.
- A55는 Linux Runtime UI/Application을 담당한다.
- Display Ownership은 M7 → HANDOVER → A55 상태기계로 관리한다.
- A55 Fault 시 M7이 Ownership을 회수하여 SAFE_DISPLAY를 수행한다.

## 3. Directory Architecture

```text
gcu3/
├── ai/
│   ├── architecture/{baseline,decisions}
│   ├── requirements/{system,software,display}
│   ├── usdm/{boot,display,ipc,fault,diagnostic}
│   ├── interface/{hal,ipc,display_link}
│   ├── verification/{test_plan,test_cases,fault_injection}
│   └── harness/{build,test,verification}
├── docs/{architecture,developer,user,verification}
├── firmware/m7/
│   ├── app/
│   ├── service/
│   ├── hal/
│   ├── display_engine/
│   ├── driver/
│   ├── bsp/
│   ├── platform/
│   ├── config/
│   ├── main.c
│   └── CMakeLists.txt
├── config/{production,development}
├── tests/{unit,integration,fault_injection,qemu}
├── tools/{build,flash,diagnostic}
└── docker/imx95-m7-builder/
```

## 4. Layer Mapping

| GCU3 | AUTOSAR conceptual mapping |
|---|---|
| app | Application SWC |
| service | Service Layer / BSW Service |
| hal | MCAL / IoHwAb concept |
| driver | MCAL / Complex Driver concept |
| bsp | ECU/MCU abstraction |
| platform | Vendor-specific BSW |
| config | ECU Configuration |
| tests | Verification |

## 5. Display Link

```text
Application
    ↓
Display Manager
    ↓
Display Link Interface
    ├── HDMI Adapter
    ├── FPD-Link Adapter
    └── GMSL Adapter
```

현재 차량용 경로는 MIPI-DSI → SDSB/SerDes 계열 Adapter → 차량용 Monitor로 추상화하며, 실제 Adapter IC/Topology는 별도 Hardware Baseline에서 확정한다.

## 6. Boot / Ownership

```text
IGN ON
  ↓
M7 Fast Boot
  ↓
Display HW Init
  ↓
Logo
  ↓
Password Input
  ↓
BOOT_COMPLETE
  ↓
A55 Boot
  ↓
A55_READY
  ↓
DISPLAY_HANDOVER
  ↓
A55_OWNER
```

Password/User Interaction 시간은 Boot Budget과 별도 관리한다.

A55 Fault:

```text
A55_OWNER
  ↓
A55_FAULT
  ↓
M7_OWNER
  ↓
SAFE_DISPLAY
```

## 7. Pre-USDM Gates

| Gate | Item | Status |
|---|---|---|
| GATE-01 | IPC/LMM feasibility | OPEN |
| GATE-05 | Secure Boot/AHAB scope | OPEN |
| GATE-02 | A55 heartbeat threshold | OPEN |
| GATE-03 | DPU driver boundary | DEFINED |
| GATE-04 | Diagnostic interface | DEFINED |
| GATE-06 | Power/Ownership linkage | DEFINED |
| GATE-07 | Build-time configuration | DEFINED |

`DEFINED`는 문서 설계가 정의되었다는 의미이며 실기/시뮬레이션 검증 완료를 의미하지 않는다. 검증 완료 시 `VERIFIED`로 승격한다.

## 8. Phase A

1. GATE-01 IPC/LMM feasibility
2. GATE-05 Secure Boot/AHAB scope
3. GATE-02 A55 heartbeat threshold
4. GATE-03 DPU driver boundary
5. GATE-04 Diagnostic interface

Phase A의 IPC/LMM 검증은 Feasibility Study이며, 정식 Runtime IPC 구현(P10)의 선행 구현이 아니다.

## 9. IPC Baseline

Runtime IPC message는 최소한 다음 개념을 가진다.

- Protocol Version
- Message Type
- Sequence
- Payload Length
- Payload
- Status/Error

Control IPC와 Runtime IPC는 분리한다. LMM/System Manager 제약은 GATE-01에서 실현 가능성을 먼저 검증한다.

## 10. Verification

Requirement → USDM → Architecture → Directory → Code → Test의 Traceability를 유지한다.

QEMU는 동일 HAL Interface Contract를 사용하여 Production과 Verification의 계약을 일치시킨다.

## 11. Security / Power / Compatibility

- Secure Boot/AHAB 범위는 GATE-05에서 확정한다.
- M7 Fast Boot time budget은 Secure Boot 결과가 확정되기 전까지 provisional이다.
- Power State와 Display Ownership의 연계를 정의한다.
- Adapter 선택은 기본적으로 Build-time Configuration이다.
- IPC Protocol Version은 Compatibility 관리 대상이다.
