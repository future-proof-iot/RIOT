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


#include "embUnit.h"

static void test_lattice_sigs(void)
{


}

Test *tests_hash_sigs(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_lattice_sigs),
    };

    EMB_UNIT_TESTCALLER(hash_lattice_tests, NULL, NULL, fixtures);
    return (Test*)&hash_lattice_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_lattice_sigs());
    TESTS_END();

    return 0;
}
