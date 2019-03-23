/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       minimal RIOT application, intended as size test
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "periph/pm.h"

#include "suit/v4/suit.h"
#include "manifest.cbor.h"
#include "riotboot/slot.h"

int main(void)
{
    char uuid_str[UUID_STR_LEN + 1];
    suit_v4_init_conditions();

    printf("running from slot %i on \"%s\"\n", riotboot_slot_current(), SUIT_CLASS_ID);

    uuid_to_string(suit_v4_get_vendor_id(), uuid_str);
    printf("SUIT vendor code: %s\n", uuid_str);
    uuid_to_string(suit_v4_get_class_id(), uuid_str);
    printf("SUIT class code: %s\n", uuid_str);
    uuid_to_string(suit_v4_get_device_id(), uuid_str);
    printf("SUIT device code: %s\n", uuid_str);
    suit_v4_manifest_t manifest;
    suit_v4_parse(&manifest, manifest_cbor, manifest_cbor_len);

    return 0;
}
