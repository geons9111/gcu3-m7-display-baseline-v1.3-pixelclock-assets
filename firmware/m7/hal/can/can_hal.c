#include "can_hal.h"
#include "flexcan_driver.h"
#include "timer_hal.h"
#include <string.h>

/* 設計ガイドラインに従い、インスタンス0、500kbpsを使用します */
static const flexcan_driver_config_t s_can_cfg = {
    .instance = 0U,
    .baudrate_bps = 500000U,
    .rx_mb_count = 8U
};

can_hal_status_t can_hal_init(void)
{
    if (flexcan_driver_init(&s_can_cfg) != 0) {
        return CAN_HAL_ERROR_BUS;
    }
    return CAN_HAL_OK;
}

can_hal_status_t can_hal_receive(can_hal_message_t *msg)
{
    if (msg == NULL) {
        return CAN_HAL_ERROR_BUS;
    }

    flexcan_frame_t frame;
    if (flexcan_driver_receive(&frame) == 0) {
        msg->can_id = frame.id;
        msg->dlc = frame.dlc;
        msg->is_extended = frame.is_extended;
        msg->timestamp_ms = timer_hal_now_ms();
        (void)memcpy(msg->data, frame.data, frame.dlc);
        return CAN_HAL_OK;
    }

    return CAN_HAL_NO_MESSAGE;
}

void can_hal_deinit(void)
{
    flexcan_driver_deinit();
}
