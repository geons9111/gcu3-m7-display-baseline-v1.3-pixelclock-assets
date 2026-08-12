# IPC Protocol v1

## Header

| Field | Size | Meaning |
|---|---:|---|
| protocol_version | 16 | Protocol compatibility |
| message_type | 16 | Message identifier |
| sequence | 32 | Monotonic sequence |
| payload_length | 16 | Payload bytes |
| status | 16 | Result/error |

## Messages

M7_READY, DISPLAY_READY, A55_READY, DISPLAY_HANDOVER, DISPLAY_ACTIVE, A55_HEARTBEAT, DISPLAY_FAULT, SAFE_DISPLAY

Transport implementation remains OPEN under GATE-01 until LMM/System Manager constraints are verified.
