#include "timer_hal.h"
#include "fsl_common.h" /* SystemCoreClock用 */

static volatile uint32_t s_ms_ticks = 0;

void SysTick_Handler(void)
{
    s_ms_ticks++;
}

void timer_hal_init(void)
{
    /* 1msの割り込みを生成するようにSysTickを初期化します。
     * 注: SystemCoreClockはBSPによって初期化されている必要があります。
     */
    SysTick_Config(SystemCoreClock / 1000U);
}

uint32_t timer_hal_now_ms(void)
{
    return s_ms_ticks;
}
