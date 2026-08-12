#include "m7_application.h"
#include "boot_manager.h"

void m7_application_init(void)
{
    boot_manager_init();
}

void m7_application_run(void)
{
    boot_manager_run();
}
