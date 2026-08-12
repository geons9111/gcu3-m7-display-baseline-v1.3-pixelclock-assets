#include "fault_manager.h"
#include "ownership_manager.h"

void fault_manager_init(void)
{
}

void fault_manager_report(fault_id_t fault)
{
    if (fault != FAULT_NONE) {
        ownership_manager_on_a55_fault();
    }
}
