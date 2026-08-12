#pragma once
#include <stdint.h>
typedef struct {
 uint16_t h_active,h_front_porch,h_sync,h_back_porch;
 uint16_t v_active,v_front_porch,v_sync,v_back_porch;
 uint32_t refresh_hz;
} gcu3_display_timing_t;
/* 候補のみ。Porch/sync値は未定（TBD）です。 */
#define GCU3_DISPLAY_TIMING_CANDIDATE {1280U,0U,0U,0U,480U,0U,0U,0U,60U}
static inline uint32_t gcu3_display_pixel_clock_hz(const gcu3_display_timing_t *t) {
 if (!t || !t->refresh_hz) return 0U;
 uint32_t h=(uint32_t)t->h_active+t->h_front_porch+t->h_sync+t->h_back_porch;
 uint32_t v=(uint32_t)t->v_active+t->v_front_porch+t->v_sync+t->v_back_porch;
 return h*v*t->refresh_hz;
}
