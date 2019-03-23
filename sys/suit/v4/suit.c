#include "uuid.h"
#include "luid.h"
#include "suit/v4/suit.h"
#include "suit/v4/policy.h"

#define SUIT_DEVID_BYTES 32

static suit_v4_condition_params_t _conditions;

void suit_v4_init_conditions(void)
{
    /* Generate UUID's following the instructions from
     * https://tools.ietf.org/html/draft-moran-suit-manifest-03#section-7.7.1
     */
    uuid_v5(&_conditions.vendor, &uuid_namespace_dns,
            (uint8_t *)SUIT_VENDOR_DOMAIN, strlen(SUIT_VENDOR_DOMAIN));

    uuid_v5(&_conditions.class, &_conditions.vendor, (uint8_t *)SUIT_CLASS_ID,
            strlen(SUIT_CLASS_ID));

    uint8_t devid[SUIT_DEVID_BYTES];
    /* Use luid_base to ensure an identical ID independent of previous luid
     * calls */
    luid_base(devid, SUIT_DEVID_BYTES);
    uuid_v5(&_conditions.device, &_conditions.vendor, devid, SUIT_DEVID_BYTES);
}

uuid_t *suit_v4_get_vendor_id(void)
{
    return &_conditions.vendor;
}

uuid_t *suit_v4_get_class_id(void)
{
    return &_conditions.class;
}

uuid_t *suit_v4_get_device_id(void)
{
    return &_conditions.device;
}

int suit_v4_policy_check(suit_v4_manifest_t *manifest)
{
    if (SUIT_DEFAULT_POLICY & ~(manifest->validated)) {
        puts("SUIT policy check failed!");
        return -1;
    }
    else {
        puts("SUIT policy check OK.");
        return 0;
    }
}
