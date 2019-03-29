#include "suit/v4/suit.h"
#include "suit/v4/policy.h"

#include "log.h"

int suit_v4_policy_check(suit_v4_manifest_t *manifest)
{
    if (SUIT_DEFAULT_POLICY & ~(manifest->validated)) {
        LOG_INFO("SUIT policy check failed!\n");
        return -1;
    }
    else {
        LOG_INFO("SUIT policy check OK.\n");
        return 0;
    }
}
