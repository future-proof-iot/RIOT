/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suitreg
 * @{
 *
 * @file
 * @brief       SUIT registry implementation
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */


#include <errno.h>
#include <string.h>

#include "assert.h"
#include "msg.h"
#include "utlist.h"
#include "suitreg.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define _INVALID_TYPE(type) (((type) > SUIT_REG_TYPE_ALL))

/**
 * @brief   Keep the head of the registers thread as global variable
 */
suit_reg_t *suit_reg = NULL;

int suit_reg_register(suit_reg_t *entry)
{
#if DEVELHELP
    bool has_msg_q = thread_has_msg_queue(sched_threads[entry->pid]);
    if (!has_msg_q) {
        LOG_ERROR("\n!!!! suit_reg: initialize message queue of thread %u "
                  "using msg_init_queue() !!!!\n\n", entry->pid);
    }
    assert(has_msg_q);
#endif /* DEVELHELP */

    if (_INVALID_TYPE(entry->type)) {
        return -EINVAL;
    }

    LL_PREPEND(suit_reg, entry);

    return 0;
}

void suit_reg_unregister(suit_reg_t *entry)
{
    if (_INVALID_TYPE(entry->type)) {
        return;
    }

    LL_DELETE(suit_reg, entry);
}

int suit_reg_notify(suit_reg_type_t reg_type, uint16_t type, uint32_t content)
{
    msg_t msg;
    msg_t msg_r;
    /* set the outgoing message's fields */
    msg.type = type;
    msg.content.value= content;

    /* get registered threads*/
    int numof = 0;
    suit_reg_t *elt = NULL;
    LL_FOREACH(suit_reg, elt) {
        if(elt->type & reg_type) {
            if (reg_type & SUIT_REG_TYPE_BLOCK & elt->type) {
                DEBUG("suit_reg: sending blocking notification");
                msg_send_receive(&msg, &msg_r, elt->pid);
            }
            else {
                DEBUG("suit_reg: sending status/error notification");
                int ret = msg_try_send(&msg, elt->pid);
                thread_yield();
                if (ret < 1) {
                    DEBUG("suit_reg: dropped message to %" PRIkernel_pid " (%s)\n", elt->pid,
                        (ret == 0) ? "receiver queue is full" : "invalid receiver");
                }
            }
            numof++;
        }

    }
    return numof;
}

/** @} */