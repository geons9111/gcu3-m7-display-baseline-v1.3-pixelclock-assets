#include "flexcan_driver.h"
#include <stddef.h>

/* Phase 1 用のスタブ実装 */

int flexcan_driver_init(const flexcan_driver_config_t *config)
{
    if (config == NULL) {
        return -1;
    }
    /* TODO: ここでMCUXpresso SDKの fsl_flexcan 関数を呼び出します */
    return 0;
}

int flexcan_driver_receive(flexcan_frame_t *frame)
{
    if (frame == NULL) {
        return -1;
    }
    /* TODO: MCUXpresso SDKの FLEXCAN_TransferReceiveNonBlocking を呼び出すか、MBステータスフラグを確認します */
    return -1; /* 現在のところ、メッセージがないことを示すために非ゼロを返します */
}

void flexcan_driver_deinit(void)
{
    /* TODO: MCUXpresso SDKの FLEXCAN_Deinit を呼び出します */
}
