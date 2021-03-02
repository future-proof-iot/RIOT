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

#include "embUnit.h"


static void test_hash_sigs(void)
{

}

Test *tests_hash_sigs(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hash_sigs),
    };

    EMB_UNIT_TESTCALLER(hash_sigs_tests, NULL, NULL, fixtures);
    return (Test*)&hash_sigs_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_hash_sigs());
    TESTS_END();

    return 0;
}
