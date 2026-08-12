#pragma once

typedef enum {
    DISPLAY_LINK_HDMI = 0,
    DISPLAY_LINK_FPD_LINK,
    DISPLAY_LINK_GMSL
} display_link_type_t;

int display_link_init(void);
int display_link_start(void);
int display_link_health_check(void);
