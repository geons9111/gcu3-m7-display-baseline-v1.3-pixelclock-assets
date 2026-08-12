#include "ownership_manager.h"
#include "ipc_manager.h"
#include "display_manager.h"

static display_owner_t owner = DISPLAY_OWNER_M7;

void ownership_manager_set_m7_owner(void)
{
    owner = DISPLAY_OWNER_M7;
}

void ownership_manager_request_a55_handover(void)
{
    owner = DISPLAY_OWNER_HANDOVER;
    ipc_manager_send_display_handover();
}

void ownership_manager_on_a55_fault(void)
{
    owner = DISPLAY_OWNER_M7;
    display_manager_show_safe_display();
    owner = DISPLAY_OWNER_SAFE;
}

display_owner_t ownership_manager_owner(void)
{
    return owner;
}
