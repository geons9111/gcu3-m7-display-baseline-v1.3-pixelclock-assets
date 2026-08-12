# GCU3 M7 Display — MIPI-DSI Pixel Clock Decision v1.3

## Decision

**Production PCLK remains OPEN/TBD.** It must not be guessed before the 7-inch monitor timing table is available.

Candidate profile:
- 1280×480
- 60 Hz
- RGB565
- 4 DSI lanes
- active pixel rate = 36.864 MHz
- active payload lower bound = 147.456 Mbps/lane

Formula:

`PCLK = (H_ACTIVE + HFP + HSYNC + HBP) × (V_ACTIVE + VFP + VSYNC + VBP) × Refresh`

Thus `GCU3_DISPLAY_PIXEL_CLOCK_HZ=0` is intentional fail-closed behavior.

The software boundary remains:

`M7 DPU → MIPI-DSI Driver/HAL → DSI-to-SerDes Adapter → Vehicle Display Link → Monitor`

The application does not access serializer registers directly.

### GATE-PCLK-01

**OPEN** until the 7-inch panel specification confirms H/V porch, sync, refresh, pixel format and DSI lane requirements.

The included RGB565 logo/safe images are verification assets, not final customer artwork.
