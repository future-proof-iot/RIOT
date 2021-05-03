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

#define ENABLE_DEBUG  (1)
#include "debug.h"

#include "falcon_api.h"

#include "embUnit.h"

#define MAXMLEN 2048
#define RUNS 2

int main(void)
{

  printf("CRYPTO_PUBLICKEYBYTES: %d\n", CRYPTO_PUBLICKEYBYTES);
	printf("CRYPTO_SECRETKEYBYTES: %d\n", CRYPTO_SECRETKEYBYTES);
	printf("CRYPTO_BYTES: %d\n", CRYPTO_BYTES);

  uint8_t sk[CRYPTO_SECRETKEYBYTES] = { 0 };
  uint8_t pk[CRYPTO_PUBLICKEYBYTES] = { 0 };

  uint8_t mi[MAXMLEN] = { 0 };
  //	uint8_t sm[MAXMLEN + CRYPTO_BYTES] = { 0 };
  uint8_t sig[CRYPTO_BYTES] = { 0 };

  printf("'sk' is at address %p\n",sk);
  printf("'pk' is at address %p\n",pk);
  printf("'mi' is at address %p\n",mi);
  printf("'sig' is at address %p\n",sig);

  //	size_t smlen;
  size_t siglen;
  //	size_t mlen;


	int r;
	size_t i;

	/* i = 0, 1, 4, 16, 64, 256, 1024 */
	for (i = 0; i < RUNS; i++) {
//randombytes(mi, 1024);



    //printf("starting crypto_sign_keypair \n");
		crypto_sign_keypair(pk, sk);
    // printf("finish crypto_sign_keypair \n");

		// printbytes(pk, CRYPTO_PUBLICKEYBYTES);
		// printbytes(sk, CRYPTO_SECRETKEYBYTES);

		//crypto_sign(sm, &smlen, mi, i, sk);
  //printf("starting crypto_sign_signature \n");
		crypto_sign_signature(sig, &siglen, mi, i, sk);
    printf("finish crypto_sign_signature \n sign lenth %d\n",siglen);

		//printbytes(sm, smlen);
		//printbytes(sig, siglen);

		// By relying on m == sm we prevent having to allocate CRYPTO_BYTES
		// twice
		r = crypto_sign_verify(sig, siglen, mi, i, pk);

		//printf("r: %d\n", r);

		if (r != 0) {
			printf("ERROR: signature verification failed\n");
			return -1;
		}
		/*for (k = 0; k < smlen; k++) {
		 if (sm[k] != mi[k]) {
		 printf("ERROR: message recovery failed\n");
		 return -1;
		 }
		 }*/
	}



    return 0;
}
