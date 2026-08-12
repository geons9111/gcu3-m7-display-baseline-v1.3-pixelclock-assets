#include "m7_application.h"
#include "boot_manager.h"
#include "health_manager.h"
#include "ipc_manager.h"
#include "timer_hal.h"

/*
 * TODO [開発戦略 2段階アプローチ]:
 * Phase 1 (現在): NXP MCUXpresso SDKサンプルコードを直接活用し、HW動作確認(Bring-up)および画面出力を優先実施。
 * Phase 2 (HW検証完了後): 動作確認済みコードの上位に Std_Types および CDD/Runnable ラッパーを実装し、AUTOSAR-Ready構造へ移行する。
 */
void m7_application_init(void)
{
    /* BSPでカバーされていないHALを初期化する */
    timer_hal_init();

    /* マネージャーを初期化する */
    boot_manager_init();
    ipc_manager_init();
    health_manager_init();
}

void m7_application_run(void)
{
    while (1) {
        if (boot_manager_state() != BOOT_STATE_BOOT_COMPLETE) {
            boot_manager_run();
        }

        ipc_manager_poll();
        health_manager_poll();
    }
}
