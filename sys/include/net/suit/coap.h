#ifndef SUIT_COAP_H
#define SUIT_COAP_H

#include "net/nanocoap.h"

void suit_coap_run(void);

extern const coap_resource_subtree_t coap_resource_subtree_suit;

#define SUIT_COAP_SUBTREE \
    { \
        .path="/suit/", \
        .methods=COAP_MATCH_SUBTREE | COAP_METHOD_GET | COAP_METHOD_POST | COAP_METHOD_PUT, \
        .handler=coap_subtree_handler, \
        .context=(void*)&coap_resource_subtree_suit \
    }

#endif /* SUIT_COAP_H */
