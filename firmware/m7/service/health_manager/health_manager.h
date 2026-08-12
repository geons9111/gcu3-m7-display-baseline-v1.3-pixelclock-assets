#pragma once

#include <stdbool.h>

void health_manager_init(void);
void health_manager_poll(void);
bool health_manager_a55_is_healthy(void);
