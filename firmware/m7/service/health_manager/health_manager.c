#include "health_manager.h"
#include "timer_hal.h"
#include "ipc_manager.h"
#include "fault_manager.h"
#include "display_config.h"

static bool a55_healthy = true;
static uint32_t last_check_ms = 0;
static uint32_t last_seq = 0;
static uint32_t miss_count = 0;

void health_manager_init(void)
{
    a55_healthy = true;
    last_check_ms = timer_hal_now_ms();
    last_seq = ipc_manager_last_heartbeat_sequence();
    miss_count = 0;
}

void health_manager_poll(void)
{
    if (!a55_healthy) {
        return;
    }

    uint32_t now = timer_hal_now_ms();
    uint32_t seq = ipc_manager_last_heartbeat_sequence();

    if (seq != last_seq) {
        last_seq = seq;
        last_check_ms = now;
        miss_count = 0;
    } else {
        if ((now - last_check_ms) > GCU3_A55_HEARTBEAT_TIMEOUT_MS) {
            miss_count++;
            last_check_ms = now; /* 次のミスインターバルのためにタイマーをリセットする */
            
            if (miss_count >= GCU3_A55_HEARTBEAT_MISS_COUNT) {
                a55_healthy = false;
                fault_manager_report(FAULT_A55_MISS_COUNT);
            }
        }
    }
}

bool health_manager_a55_is_healthy(void)
{
    return a55_healthy;
}
