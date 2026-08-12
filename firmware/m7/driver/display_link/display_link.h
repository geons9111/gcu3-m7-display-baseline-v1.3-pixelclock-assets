#pragma once

typedef enum {
    DISPLAY_LINK_HDMI = 0,
    DISPLAY_LINK_FPD_LINK,
    DISPLAY_LINK_GMSL
} display_link_type_t;

typedef struct {
    int (*init)(void);
    int (*start)(void);
    int (*stop)(void);
    int (*health_check)(void);
} display_link_ops_t;

int display_link_init(void);
int display_link_start(void);
int display_link_health_check(void);
