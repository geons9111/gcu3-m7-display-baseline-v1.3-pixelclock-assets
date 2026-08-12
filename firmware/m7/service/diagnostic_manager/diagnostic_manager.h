#pragma once

typedef struct {
    unsigned int id;
    unsigned int status;
} diagnostic_event_t;

void diagnostic_manager_init(void);
void diagnostic_manager_report(diagnostic_event_t event);
