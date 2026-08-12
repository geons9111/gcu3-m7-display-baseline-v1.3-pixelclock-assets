#include <assert.h>
#include "ownership_manager.h"

int main(void)
{
    ownership_manager_set_m7_owner();
    assert(ownership_manager_owner() == DISPLAY_OWNER_M7);
    ownership_manager_request_a55_handover();
    assert(ownership_manager_owner() == DISPLAY_OWNER_HANDOVER);
    return 0;
}
