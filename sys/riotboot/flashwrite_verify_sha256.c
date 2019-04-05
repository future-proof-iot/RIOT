#include <stdint.h>
#include <string.h>

#include "hashes/sha256.h"

#include "riotboot/slot.h"

#include "log.h"

int riotboot_flashwrite_verify_sha256(const uint8_t *sha256_digest, size_t img_len, int target_slot)
{
    char digest[SHA256_DIGEST_LENGTH];

    sha256_context_t sha256;

    if (img_len < 4) {
        LOG_INFO("riotboot: verify_sha256(): image too small\n");
        return -1;
    }

    uint8_t *img_start = (uint8_t *)riotboot_slot_get_hdr(target_slot);

    LOG_INFO("riotboot: verifying digest at %p (img at: %p size: %u)\n", sha256_digest, img_start, img_len);

    sha256_init(&sha256);
    sha256_update(&sha256, "RIOT", 4);
    sha256_update(&sha256, img_start + 4, img_len - 4);
    sha256_final(&sha256, digest);

    return memcmp(sha256_digest, digest, SHA256_DIGEST_LENGTH) == 0;
}
