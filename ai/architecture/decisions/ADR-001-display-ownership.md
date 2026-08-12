# ADR-001 Display Ownership

Status: Accepted

M7이 Fast Boot 단계에서 Display Ownership을 보유하고 A55_READY 이후 명시적인 HANDOVER를 수행한다. A55 Fault 시 M7이 Ownership을 회수하여 SAFE_DISPLAY를 수행한다.
