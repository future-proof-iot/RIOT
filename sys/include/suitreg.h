
/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_suitreg SUIT registry
 * @ingroup     sys
 * @brief       Global sensor/actuator registry for SUIT devices
 *
 * @{
 *
 * @file
 * @brief       SUIT registry interface definition
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef SUITREG_H
#define SUITREG_H

#include <stdint.h>
#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SUIT status messages extend as needed
 */
enum {
    SUIT_TRIGGER = 0x07A00,      /**< trigger has been received */
    SUIT_SIGNATURE_START,        /**< manifest signature validation will start */
    SUIT_SIGNATURE_ERROR,        /**< manifest signature error */
    SUIT_SIGNATURE_END,          /**< manifest signature end */
    SUIT_DOWNLOAD_START,         /**< new image download will start */
    SUIT_DOWNLOAD_PROGRESS,      /**< image download progress report */
    SUIT_DOWNLOAD_ERROR,         /**< image download error */
    SUIT_DIGEST_START,           /**< digest validation will start */
    SUIT_DIGEST_ERROR,           /**< digest validation error */
    SUIT_DOWNLOAD_END,           /**< image ended with success */
    SUIT_REBOOT,                 /**< suit is going to reboot device*/
};

/**
 * @brief   Generate REG_TYPE bitfield
 *
 * - bit 0: subscribe to error messages
 * - bit 1: subscribe to status messages
 * - bit 2: subscribe to blocking messages
 */
#define REG_TYPE(blk, sts, err)   ((blk << 3) | (sts << 2) | (err << 0))

/**
 * @brief   SUIT subscription types
 * 
 * @{
 */
typedef enum {
    SUITREG_TYPE_NONE     = REG_TYPE(0, 0, 0),  /**< no messages */
    SUITREG_TYPE_ERROR    = REG_TYPE(0, 0, 1),  /**< only errors updates*/
    SUITREG_TYPE_STATUS   = REG_TYPE(0, 1, 0),  /**< only status updates */
    SUITREG_TYPE_BLOCK    = REG_TYPE(1, 0, 0),  /**< only blocking updates */
    SUITREG_TYPE_ALL      = REG_TYPE(1, 1, 1),  /**< all updates */
} suitreg_type_t;
/** @} */

/**
 * @brief   SUIT registry entry
 */
typedef struct suitreg {
    struct suitreg* next;          /**< pointer to next registry entry */
    suitreg_type_t type;           /**< registry type */
    kernel_pid_t pid;               /**< pid of registered thread */
} suitreg_t;

/**
 * @name    Static entry initialization macros
 * @anchor  suitreg_init_static
 * @{
 */
/**
 * @brief   Initializes a suitreg entry statically with PID
 *
 * @param[in] type      The @ref suitreg_type_t for the suitreg entry
 * @param[in] pid       The PID of the registering thread
 *
 * @return  An initialized suitreg entry
 */
#define SUITREG_INIT_PID(type, pid)  { NULL, type, pid }

/**
 * @name    Dynamic entry initialization functions
 * @anchor  suitreg_init_dyn
 * @{
 */
/**
 * @brief   Initializes a suitreg entry dynamically with PID
 *
 * @param[out] entry    A suitreg entry
 * @param[in] type      The @ref suitreg_type_t for the suitreg entry
 * @param[in] pid       The PID of the registering thread
 *
 */
static inline void suitreg_init_pid(suitreg_t *entry,
                                     suitreg_type_t type,
                                     kernel_pid_t pid)
{
    entry->next = NULL;
    entry->type = type;
    entry->pid = pid;
}

/**
 * @brief   Registers a thread to the registry.
 *
 * @param[in] entry     An entry you want to add to the registry. This needs to
 *                      be initialized before hand using the @ref
 *                      suitreg_init_static "static" or @ref suitreg_init_static
 *                      "dynamic" initialization helpers.
 *
 * @warning Call suitreg_unregister() *before* you leave the context you
 *          allocated @p entry in. Otherwise it might get overwritten.
 *
 * @pre The calling thread must provide a [message queue](@ref msg_init_queue)
 *      when using @ref GNRC_suitreg_TYPE_DEFAULT for gnrc_suitreg_entry_t::type
 *      of @p entry.
 *
 * @return  0 on success
 * @return  -EINVAL if invalid entry
 */
int suitreg_register(suitreg_t *entry);

/**
 * @brief   Removes a thread from the registry.
 *
 * @param[in] entry     An entry you want to remove from the registry.
 */
void suitreg_unregister(suitreg_t *entry);

/**
 * @brief   Sends a msg to all subscriber to @p type.
 *
 * @param[in] reg_type   The @ref suitreg_type_t
 * @param[in] type       The message type. 
 * @param[in] content    Content of the message, can also be a pointer to a structure
 * 
 * @return Number of subscribers to (@p type).
 */
int suitreg_notify(suitreg_type_t reg_type, uint16_t type, uint32_t content);

/**
 * @brief   Export the SUIT registry as global variable
 */
extern suitreg_t *suitreg;


#ifdef __cplusplus
}
#endif

#endif /* SUITREG_H */