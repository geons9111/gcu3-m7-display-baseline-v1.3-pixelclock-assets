#include "health_manager.h"

static bool a55_healthy = true;

void health_manager_init(void)
{
    a55_healthy = true;
}

void health_manager_poll(void)
{
    /* GATE-02: threshold values remain configuration items until verified. */
}

bool health_manager_a55_is_healthy(void)
{
    return a55_healthy;
}
