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

## Important

The current values in `display_config.h` marked `provisional` are placeholders and are not production calibration values.
