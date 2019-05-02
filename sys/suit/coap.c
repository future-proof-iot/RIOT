/*
 * Copyright (C) 2019 Freie Universität Berlin
 *               2019 Inria
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <inttypes.h>
#include <string.h>

#include "msg.h"
#include "log.h"
#include "net/nanocoap.h"
#include "net/nanocoap_sock.h"
#include "thread.h"
#include "periph/pm.h"

#ifdef MODULE_RIOTBOOT_SLOT
#include "riotboot/slot.h"
#include "riotboot/flashwrite.h"
#endif

#ifdef MODULE_SUIT_V4
#include "suit/v4/suit.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef SUIT_COAP_STACKSIZE
#define SUIT_COAP_STACKSIZE (3*THREAD_STACKSIZE_LARGE)
#endif

#ifndef SUIT_COAP_PRIO
#define SUIT_COAP_PRIO THREAD_PRIORITY_MAIN - 1
#endif

#define SUIT_URL_MAX            128
#define SUIT_MANIFEST_BUFSIZE   640
#define SUIT_MSG_TRIGGER        0x12345

static char _stack[SUIT_COAP_STACKSIZE];
static char _url[SUIT_URL_MAX];
static uint8_t _manifest_buf[SUIT_MANIFEST_BUFSIZE];

static kernel_pid_t _suit_coap_pid;

static void _suit_handle_url(const char *url)
{
    LOG_INFO("suit_coap: downloading \"%s\"\n", url);
    ssize_t size = nanocoap_get_blockwise_url_buf(url, COAP_BLOCKSIZE_64, _manifest_buf,
                                              SUIT_MANIFEST_BUFSIZE);
    if (size >= 0) {
        LOG_INFO("suit_coap: got manifest with size %u\n", (unsigned)size);

        riotboot_flashwrite_t writer;
#ifdef MODULE_SUIT_V1
        suit_v1_cbor_manifest_t manifest_v1;
        ssize_t res;

        if ((res = suit_v1_parse(&manifest_v1, _manifest_buf, size)) != SUIT_OK) {
            printf("suit_v1_parse() failed. res=%i\n", res);
            return;
        }

        if ((res = suit_v1_cbor_get_url(&manifest_v1, _url, SUIT_URL_MAX -1)) <= 0) {
            printf("suit_v1_cbor_get_url() failed res=%i\n", res);
            return;
        }

        assert (res < SUIT_URL_MAX);
        _url[res] = '\0';

        LOG_INFO("suit_coap: got image URL(len=%u): \"%s\"\n", (unsigned)res, _url);
        riotboot_flashwrite_init(&writer, riotboot_slot_other());
        res = nanocoap_get_blockwise_url(_url, COAP_BLOCKSIZE_64, suit_flashwrite_helper,
                                         &writer);
#else
        suit_v4_manifest_t manifest;
        memset(&writer, 0, sizeof(manifest));

        manifest.writer = &writer;
        manifest.urlbuf = _url;
        manifest.urlbuf_len = SUIT_URL_MAX;

        ssize_t res;
        if ((res = suit_v4_parse(&manifest, _manifest_buf, size)) != SUIT_OK) {
            printf("suit_v4_parse() failed. res=%i\n", res);
            return;
        }

        printf("suit_v4_parse() success\n");
        if (!(manifest.state & SUIT_MANIFEST_HAVE_IMAGE)) {
            puts("manifest parsed, but no image fetched");
            return;
        }

        res = suit_v4_policy_check(&manifest);
        if (res) {
            return;
        }

#endif
        if (res == 0) {
            LOG_INFO("suit_coap: finalizing image flash\n");
            riotboot_flashwrite_finish(&writer);

            const riotboot_hdr_t *hdr = riotboot_slot_get_hdr(riotboot_slot_other());
            riotboot_hdr_print(hdr);
            xtimer_sleep(1);

            if (riotboot_hdr_validate(hdr) == 0) {
                LOG_INFO("suit_coap: rebooting...");
                pm_reboot();
            }
            else {
                LOG_INFO("suit_coap: update failed, hdr invalid");
            }
        }
    }
    else {
        LOG_INFO("suit_coap: error getting manifest\n");
    }
}

int suit_flashwrite_helper(void *arg, size_t offset, uint8_t *buf, size_t len,
                    int more)
{
    riotboot_flashwrite_t *writer = arg;

    if (offset == 0) {
        if (len < RIOTBOOT_FLASHWRITE_SKIPLEN) {
            LOG_WARNING("_suit_flashwrite(): offset==0, len<4. aborting\n");
            return -1;
        }
        offset = RIOTBOOT_FLASHWRITE_SKIPLEN;
        buf += RIOTBOOT_FLASHWRITE_SKIPLEN;
        len -= RIOTBOOT_FLASHWRITE_SKIPLEN;
    }

    if (writer->offset != offset) {
        LOG_WARNING("_suit_flashwrite(): writer->offset=%u, offset==%u, aborting\n",
                    (unsigned)writer->offset, (unsigned)offset);
        return -1;
    }

    DEBUG("_suit_flashwrite(): writing %u bytes at pos %u\n", len, offset);

    return riotboot_flashwrite_putbytes(writer, buf, len, more);
}

static void *_suit_coap_thread(void *arg)
{
    (void)arg;

    LOG_INFO("suit_coap: started.\n");
    msg_t msg_queue[4];
    msg_init_queue(msg_queue, 4);

    _suit_coap_pid = thread_getpid();

    msg_t m;
    while (true) {
        msg_receive(&m);
        DEBUG("suit_coap: got msg with type %" PRIu32 "\n", m.content.value);
        switch (m.content.value) {
            case SUIT_MSG_TRIGGER:
                LOG_INFO("suit_coap: trigger received\n");
                _suit_handle_url(_url);
                break;
            default:
                LOG_WARNING("suit_coap: warning: unhandled msg\n");
        }
    }
    return NULL;
}

void suit_coap_run(void)
{
    thread_create(_stack, SUIT_COAP_STACKSIZE, SUIT_COAP_PRIO,
                  THREAD_CREATE_STACKTEST,
                  _suit_coap_thread, NULL, "suit_coap");
}

static ssize_t _version_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                                void *context)
{
    (void)context;
    return coap_reply_simple(pkt, COAP_CODE_205, buf, len,
                             COAP_FORMAT_TEXT, (uint8_t *)"NONE", 4);
}

#ifdef MODULE_RIOTBOOT_SLOT
static ssize_t _slot_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                                void *context)
{
    /* context is passed either as NULL or 0x1 for /active or /inactive */
    char c = '0';
    if (context) {
        c += riotboot_slot_other();
    }
    else {
        c += riotboot_slot_current();
    }

    return coap_reply_simple(pkt, COAP_CODE_205, buf, len,
                             COAP_FORMAT_TEXT, (uint8_t *)&c, 1);
}
#endif

static ssize_t _trigger_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                                void *context)
{
    (void)context;
    unsigned code;
    size_t payload_len = pkt->payload_len;
    if (payload_len) {
        if (payload_len >= SUIT_URL_MAX) {
            code = COAP_CODE_REQUEST_ENTITY_TOO_LARGE;
        }
        else {
            memcpy(_url, pkt->payload, payload_len);
            _url[payload_len] = '\0';

            code = COAP_CODE_CREATED;
            LOG_INFO("suit: received URL: \"%s\"\n", _url);
            msg_t m = { .content.value = SUIT_MSG_TRIGGER };
            msg_send(&m, _suit_coap_pid);
        }
    }
    else {
        code = COAP_CODE_REQUEST_ENTITY_INCOMPLETE;
    }

    return coap_reply_simple(pkt, code, buf, len,
                             COAP_FORMAT_NONE, NULL, 0);
}

static const coap_resource_t _subtree[] = {
#ifdef MODULE_RIOTBOOT_SLOT
    { "/suit/slot/active", COAP_METHOD_GET, _slot_handler, NULL },
    { "/suit/slot/inactive", COAP_METHOD_GET, _slot_handler, (void*)0x1 },
#endif
    { "/suit/trigger", COAP_METHOD_PUT | COAP_METHOD_POST, _trigger_handler, NULL },
    { "/suit/version", COAP_METHOD_GET, _version_handler, NULL },
};

const coap_resource_subtree_t coap_resource_subtree_suit =
{
    .resources = &_subtree[0],
    .resources_numof = sizeof(_subtree)/sizeof(_subtree[0])
};
