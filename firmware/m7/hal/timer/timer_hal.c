#include "timer_hal.h"
#include "fsl_common.h" /* For SystemCoreClock */

static volatile uint32_t s_ms_ticks = 0;

void SysTick_Handler(void)
{
    s_ms_ticks++;
}

void timer_hal_init(void)
{
    /* Initialize SysTick to generate 1ms interrupts.
     * Note: SystemCoreClock must be initialized by the BSP.
     */
    SysTick_Config(SystemCoreClock / 1000U);
}

uint32_t timer_hal_now_ms(void)
{
    return s_ms_ticks;
}
