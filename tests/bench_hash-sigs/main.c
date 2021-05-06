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
 * @brief       Tests hash-sigs package
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>

#include "xtimer.h"
#include "hash_sig_api.h"

#include "embUnit.h"

#define MESSAGE_SIZE 17

char _stack[HASH_SIGS_STACKSIZE];

static const uint8_t sk[PRIVATE_KEY_LEN] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
			0x0, 0x53, 0x53, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x59, 0x68,
			0x3a, 0xca, 0xc8, 0xfe, 0x75, 0x5a, 0x30, 0x7e, 0xbf, 0x2d, 0xb4,
			0xb9, 0xee, 0xf0, 0xd9, 0xb1, 0xa9, 0x2c, 0xa6, 0x5f, 0xb2, 0x6b,
			0xf0, 0x4f, 0xed, 0x80, 0x44, 0x36, 0x1, 0x4a, 0x83, 0xbf, 0xbd,
			0x5e, 0x7f, 0xaa, 0x6f, 0xbd, 0xdc, 0x6f, 0xfb, 0xe6, 0xc, 0x38,
			0x17, 0x9d };
static const uint8_t pk[HSS_MAX_PUBLIC_KEY_LEN] = { 0x0, 0x0, 0x0, 0x2, 0x0, 0x0,
			0x0, 0x5, 0x0, 0x0, 0x0, 0x3, 0x83, 0xbf, 0xbd, 0x5e, 0x7f, 0xaa,
			0x6f, 0xbd, 0xdc, 0x6f, 0xfb, 0xe6, 0xc, 0x38, 0x17, 0x9d, 0x8f,
			0x90, 0x1d, 0x46, 0x6f, 0x92, 0xcd, 0xe8, 0x21, 0x14, 0x91, 0xe3,
			0xe3, 0xcc, 0x3c, 0x92, 0xf5, 0x7b, 0xc6, 0xbe, 0xb9, 0x42, 0xb5,
			0x65, 0x5a, 0xcd, 0x8d, 0xf1, 0xc7, 0xc3, 0x7, 0x65 };
static const uint8_t message[MESSAGE_SIZE] = "This should work!";
uint8_t signature[CRYPTO_BYTES + MESSAGE_SIZE];
size_t length = 0;

uint32_t start, stop;

void *_sign(void* arg)
{
    (void)arg;
    start = xtimer_now_usec();
    sign(signature, (uint64_t*)&length, (uint8_t*)message, MESSAGE_SIZE, (uint8_t*)sk);
    stop = xtimer_now_usec();
    return NULL;
}

void *_verify(void* arg)
{
    (void)arg;
    start = xtimer_now_usec();
	verify((uint8_t*)pk, signature, (uint64_t)length, (uint8_t*)message, MESSAGE_SIZE);
    stop = xtimer_now_usec();
    return NULL;
}

int main(void)
{

    thread_create(_stack,
                                       sizeof(_stack),
                                       (THREAD_PRIORITY_MAIN - 1),
                                       THREAD_CREATE_STACKTEST,
                                       _sign,
                                       NULL,
                                       "sign");

    uint32_t sign_time = stop - start;
    size_t sign_stack_use = sizeof(_stack) - thread_measure_stack_free(_stack);


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
          CRYPTO_BYTES);
}

