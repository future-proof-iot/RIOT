/*
 * Copyright (C) 2021 Koen Zandberg
 *               2021 Inria
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
 * @brief       benchmarks tinycrypt package
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>

#include "xtimer.h"
#include "random.h"
#include "tinycrypt/ecc.h"
#include "tinycrypt/ecc_dh.h"
#include "tinycrypt/ecc_dsa.h"
#include "tinycrypt/sha256.h"

#define MESSAGE_SIZE 17

static char _stack[TINYCRYPT_SIGS_STACKSIZE];
static uint8_t sk[NUM_ECC_BYTES];
static uint8_t pk[2 * NUM_ECC_BYTES];
static uint8_t sig[2 * NUM_ECC_BYTES];

static const uint8_t message[MESSAGE_SIZE] = "This should work!";

size_t siglen = 0;

uint32_t start, stop;

int default_CSPRNG(uint8_t *buf, unsigned int size)
{
    random_bytes(buf, (size_t)size);
    return 1;
}

static void *_sign(void* arg)
{
    (void)arg;
    start = xtimer_now_usec();
    uint8_t digest[TC_SHA256_DIGEST_SIZE];
    struct tc_sha256_state_struct sha256;
    tc_sha256_init(&sha256);
    tc_sha256_update(&sha256, message, sizeof(message));
    tc_sha256_final(digest, &sha256);

    uECC_sign(sk, digest, sizeof(digest), sig, uECC_secp256r1());
    stop = xtimer_now_usec();
    return NULL;
}

static void *_verify(void* arg)
{
    (void)arg;
    start = xtimer_now_usec();

    uint8_t digest[32];
    struct tc_sha256_state_struct sha256;
    tc_sha256_init(&sha256);
    tc_sha256_update(&sha256, message, sizeof(message));
    tc_sha256_final(digest, &sha256);

    int res = uECC_verify(pk, digest, sizeof(digest), sig, uECC_secp256r1());

    stop = xtimer_now_usec();
    assert(res);
    return NULL;
}

int main(void)
{
    uECC_make_key(pk, sk, uECC_secp256r1());

    printf("Start sign\n");
    thread_create(_stack,
                  sizeof(_stack),
                  (THREAD_PRIORITY_MAIN - 1),
                  THREAD_CREATE_STACKTEST,
                  _sign,
                  NULL,
                  "sign");

    uint32_t sign_time = stop - start;
    size_t sign_stack_use = sizeof(_stack) - thread_measure_stack_free(_stack);
    printf("Start verify\n");

    thread_create(_stack,
                  sizeof(_stack),
                  (THREAD_PRIORITY_MAIN - 1),
                  THREAD_CREATE_STACKTEST,
                  _verify,
                  NULL,
                  "verify");

    uint32_t verify_time = stop - start;
    size_t verify_stack_use = sizeof(_stack) - thread_measure_stack_free(_stack);

    printf("{ \"result\": { \"sign\": %lums, \"verify\": %lums "
#ifdef CLOCK_CORECLOCK
           "\"sign_ticks\": %"PRIu32", \"verify_ticks\": %"PRIu32" "
#endif
           "\"sign_stack\": %"PRIu32"B \"verify_stack\": %"PRIu32"B "
           "\"signature_size\": %dB } }\n",
          sign_time/US_PER_MS, verify_time/US_PER_MS,
#ifdef CLOCK_CORECLOCK
          (uint32_t)(((uint64_t)sign_time * CLOCK_CORECLOCK)/US_PER_SEC),
          (uint32_t)(((uint64_t)verify_time * CLOCK_CORECLOCK)/US_PER_SEC),
#endif
          (uint32_t)sign_stack_use, (uint32_t)verify_stack_use,
          2 * NUM_ECC_BYTES);
}
