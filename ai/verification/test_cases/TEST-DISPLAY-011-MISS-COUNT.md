# TEST-DISPLAY-011 A55 Heartbeat Miss Count

## Objective
Verify the MISS_COUNT branch independently from HEARTBEAT_TIMEOUT and IPC_CHANNEL_FAILURE.

## Preconditions
- M7 Health Manager active
- A55 heartbeat enabled
- MISS_COUNT configured

## Steps
1. Suppress exactly N heartbeat messages.
2. Verify no fault before threshold.
3. Reach threshold N.
4. Verify A55 fault detection.
5. Verify M7 ownership recovery and SAFE_DISPLAY.

## Expected Result
Only the configured MISS_COUNT threshold causes the corresponding fault transition.
