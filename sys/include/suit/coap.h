#ifndef SUIT_COAP_H
#define SUIT_COAP_H

#include "net/nanocoap.h"

void suit_coap_run(void);

/* subtree support start */
ssize_t coap_subtree_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                             void *context);

/**
 * @brief   Type for CoAP resource subtrees
 */
typedef const struct {
    const coap_resource_t *resources;   /**< ptr to resource array  */
    const size_t resources_numof;       /**< nr of entries in array */
} coap_resource_subtree_t;

typedef int (*coap_blockwise_cb_t)(void *arg, size_t offset, uint8_t *buf, size_t len, int more);

#define SUIT_COAP_SUBTREE \
    { \
        .path="/suit/", \
        .methods=COAP_MATCH_SUBTREE | COAP_METHOD_GET | COAP_METHOD_POST | COAP_METHOD_PUT, \
        .handler=coap_subtree_handler, \
        .context=(void*)&coap_resource_subtree_suit \
    }

extern const coap_resource_subtree_t coap_resource_subtree_suit;
/* subtree support end */

/* nanocoap blockwise client start */
typedef enum {
    COAP_BLOCKSIZE_32 = 1,
    COAP_BLOCKSIZE_64,
    COAP_BLOCKSIZE_128,
    COAP_BLOCKSIZE_256,
    COAP_BLOCKSIZE_512,
    COAP_BLOCKSIZE_1024,
} coap_blksize_t;

size_t coap_put_option_block(uint8_t *buf, uint16_t lastonum, unsigned blknum,
                             unsigned szx, int more, uint16_t option);

int suit_coap_get_blockwise_url(const char *url,
                               coap_blksize_t blksize,
                               coap_blockwise_cb_t callback, void *arg);

/* nanocoap blockwise client end */

#endif /* SUIT_COAP_H */
