/*
 * Copyright (C) 2021 Gustavo Banegas
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
 * @brief       Tests lattice-sigs package
 *
 * @author      Gustavo Banegas <gustavo@cryptme.in>
 *
 * @}
 */

#include <string.h>

#include "lattice_sign_api.h"

#include "embUnit.h"

#define MAXMLEN 2048


static void test_lattice_sigs(void)
{

  uint8_t sk[CRYPTO_SECRETKEYBYTES];
      uint8_t pk[CRYPTO_PUBLICKEYBYTES];

      uint8_t mi[MAXMLEN] = {0x52, 0x49, 0x4f, 0x54, 0x21};
      uint8_t sm[MAXMLEN + CRYPTO_BYTES];
      uint8_t sig[CRYPTO_BYTES];

      size_t smlen;
      size_t siglen;
      size_t mlen;

      int r;
      size_t i;
      /* i = 0, 1, 4, 16, 64, 256, 1024 */
      for (i = 0; i < MAXMLEN; i = (i == 0) ? i + 1 : i << 2) {
          //randombytes(mi, i);
          crypto_sign_keypair(pk, sk);

          crypto_sign(sm, &smlen, mi, i, sk);
          crypto_sign_signature(sig, &siglen, mi, i, sk);
          // By relying on m == sm we prevent having to allocate CRYPTO_BYTES
          // twice
          r = crypto_sign_open(sm, &mlen, sm, smlen, pk);
          r |= crypto_sign_verify(sig, siglen, mi, i, pk);

          TEST_ASSERT_EQUAL_INT(r, 0);

          /*if (r) {
              printf("ERROR: signature verification failed\n");
              return -1;
          }*/
          /*for (k = 0; k < i; k++) {
              if (sm[k] != mi[k]) {
                  printf("ERROR: message recovery failed\n");
                  return -1;
              }
          }*/
      }

}

Test *tests_lattices_sigs(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_lattice_sigs)
    };

    EMB_UNIT_TESTCALLER(lattice_sig_tests, NULL, NULL, fixtures);
    return (Test*)&lattice_sig_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_lattices_sigs());
    TESTS_END();

    return 0;
}
