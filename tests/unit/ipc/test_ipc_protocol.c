#include <assert.h>
#define IPC_PROTOCOL_VERSION 1U
int main(void)
{
    assert(IPC_PROTOCOL_VERSION == 1U);
    return 0;
}
