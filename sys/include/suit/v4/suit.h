/*
 * Copyright (C) 2019 Koen Zandberg
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_suit_v4 SUIT draft v4
 * @ingroup     sys_suit
 * @brief       SUIT manifest handling
 *
 * @{
 *
 * @brief       Handler functions for SUIT manifests
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#ifndef SUIT_V4_SUIT_H
#define SUIT_V4_SUIT_H

#include <stddef.h>
#include <stdint.h>

#include "cose/sign.h"
#include "cbor.h"
#include "uuid.h"
#include "riotboot/flashwrite.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SUIT_COSE_BUF_SIZE
#define SUIT_COSE_BUF_SIZE      512
#endif

#define SUIT_V4_COMPONENT_MAX 1

/**
 * @brief Supported SUIT manifest version
 */
#define SUIT_MANIFEST_VERSION        4
#define SUIT_VERSION        1

/**
 * @brief SUIT error codes
 */
typedef enum {
    SUIT_OK                     = 0,    /**< Manifest parsed and validated */
    SUIT_ERR_INVALID_MANIFEST   = -1,   /**< Unexpected CBOR structure detected */
    SUIT_ERR_UNSUPPORTED        = -2,   /**< Unsupported SUIT feature detected */
    SUIT_ERR_NOT_SUPPORTED      = -3,   /**< Unsupported manifest features detected */
    SUIT_ERR_COND               = -4,   /**< Conditionals evaluate to false */
    SUIT_ERR_SEQUENCE_NUMBER    = -5,   /**< Sequence number less or equal to
                                             current sequence number */
    SUIT_ERR_SIGNATURE          = -6,   /**< Unable to verify signature */
} suit_v4_error_t;

/**
 * @brief TinyCBOR validation mode to use
 */
#define SUIT_TINYCBOR_VALIDATION_MODE       CborValidateStrictMode

/**
 * @brief SUIT payload digest algorithms
 *
 * Unofficial list from
 * [suit-manifest-generator](https://github.com/ARMmbed/suit-manifest-generator)
 */
typedef enum {
    SUIT_DIGEST_NONE    = 0,    /**< No digest algo supplied */
    SUIT_DIGEST_SHA256  = 1,    /**< SHA256 */
    SUIT_DIGEST_SHA384  = 2,    /**< SHA384 */
    SUIT_DIGEST_SHA512  = 3,    /**< SHA512 */
} suit_v4_digest_t;

/**
 * @brief SUIT payload digest types
 *
 * Unofficial list from
 * [suit-manifest-generator](https://github.com/ARMmbed/suit-manifest-generator)
 */
typedef enum {
    SUIT_DIGEST_TYPE_RAW        = 1,    /**< Raw payload digest */
    SUIT_DIGEST_TYPE_INSTALLED  = 2,    /**< Installed firmware digest */
    SUIT_DIGEST_TYPE_CIPHERTEXT = 3,    /**< Ciphertext digest */
    SUIT_DIGEST_TYPE_PREIMAGE   = 4     /**< Pre-image digest */
} suit_v4_digest_type_t;

enum {
    SUIT_COMPONENT_IDENTIFIER   = 1,
    SUIT_COMPONENT_SIZE         = 2,
    SUIT_COMPONENT_DIGEST       = 3,
};

/**
 * @brief SUIT v4 component struct
 */
typedef struct {
    uint32_t size;
    CborValue identifier;
    CborValue url;
    CborValue digest;
} suit_v4_component_t;

/**
 * @brief SUIT manifest struct
 */
typedef struct {
    cose_sign_t cose;   /**< COSE signature validation struct */
    const uint8_t *buf; /**< ptr to the buffer of the manifest */
    size_t len;         /**< length of the manifest */
    uint32_t validated; /**< bitfield of validated policies */
    uint32_t state;     /**< bitfield holding state information */

    suit_v4_component_t components[SUIT_V4_COMPONENT_MAX];
    //CborValue components[SUIT_V4_COMPONENT_MAX];
    unsigned components_len;
    int component_current;
    riotboot_flashwrite_t *writer;
    uint8_t validation_buf[SUIT_COSE_BUF_SIZE];
    cose_key_t *key;    /**< Ptr to the public key for validation */
    char *urlbuf;
    size_t urlbuf_len;
} suit_v4_manifest_t;

#define SUIT_MANIFEST_HAVE_COMPONENTS 0x1
#define SUIT_MANIFEST_HAVE_IMAGE 0x2

/**
 * @brief Parse a manifest
 *
 * @note The buffer is still required after parsing, please don't reuse the
 * buffer while the @p manifest is used
 *
 * @param   manifest    manifest context to store information in
 * @param   buf         buffer to parse the manifest from
 * @param   len         length of the manifest data in the buffer
 *
 * @return              SUIT_OK on parseable manifest
 * @return              negative @ref suit_v4_error_t code on error
 */
int suit_v4_parse(suit_v4_manifest_t *manifest, const uint8_t *buf, size_t len);

int suit_v4_policy_check(suit_v4_manifest_t *manifest);


int suit_cbor_map_iterate_init(CborValue *map, CborValue *it);
int suit_cbor_map_iterate(CborValue *map, CborValue *key, CborValue *value);

int suit_cbor_get_int(const CborValue *key, int *out);
int suit_cbor_get_uint(const CborValue *key, unsigned *out);
int suit_cbor_get_uint32(const CborValue *it, uint32_t *out);
int suit_cbor_get_string(const CborValue *it, const uint8_t **buf, size_t *len);
int suit_cbor_subparse(CborParser *parser, CborValue *bseq, CborValue *it);

int suit_flashwrite_helper(void *arg, size_t offset, uint8_t *buf, size_t len,
                    int more);

#ifdef __cplusplus
}
#endif

#endif /* SUIT_V4_SUIT_H */
/** @} */
