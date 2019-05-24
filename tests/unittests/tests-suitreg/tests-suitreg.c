/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdint.h>

#include "embUnit/embUnit.h"
#include "unittests-constants.h"
#include "utlist.h"

#include "suitreg.h"
#include "tests-suitreg.h"

static suitreg_t entries[] = {
    SUITREG_INIT_PID(SUITREG_TYPE_ALL+1, TEST_UINT8),
    SUITREG_INIT_PID(SUITREG_TYPE_ALL, TEST_UINT8),
    SUITREG_INIT_PID(SUITREG_TYPE_ALL, TEST_UINT8+1),
    SUITREG_INIT_PID(SUITREG_TYPE_ALL, TEST_UINT8+2),
};

static suitreg_t *_suitreg_lookup_pid(suitreg_t *from,
                                        kernel_pid_t pid)
{
    suitreg_t *res = NULL;

    suitreg_t *head = (from) ? from->next : suitreg;
    LL_SEARCH_SCALAR(head, res, pid, pid);    

    return res;
}

static suitreg_t *suitreg_lookup_pid(kernel_pid_t pid)
{
    return _suitreg_lookup_pid(NULL, pid);
}

static suitreg_t *suitreg_getnext(suitreg_t *entry)
{
    return (entry ? _suitreg_lookup_pid(entry, entry->pid) : NULL);
}

static void set_up(void)
{
    suitreg = NULL;
}

static void test_suitreg_register__inval(void)
{
    TEST_ASSERT_EQUAL_INT(-EINVAL, suitreg_register(&entries[0]));
}

static void test_suitreg_register__success(void)
{
    suitreg_t *res = suitreg_lookup_pid(TEST_UINT8);

    TEST_ASSERT_NULL(res);

    TEST_ASSERT_EQUAL_INT(0, suitreg_register(&entries[1]));
    TEST_ASSERT_NOT_NULL((res = suitreg_lookup_pid(TEST_UINT8)));
    TEST_ASSERT_EQUAL_INT(SUITREG_TYPE_ALL, res->type);
    TEST_ASSERT_EQUAL_INT(TEST_UINT8, res->pid);
    TEST_ASSERT_NULL((suitreg_getnext(res)));
}

void test_suitreg_unregister__success(void)
{
    TEST_ASSERT_EQUAL_INT(0, suitreg_register(&entries[1]));
    TEST_ASSERT_NOT_NULL(suitreg_lookup_pid(TEST_UINT8));
    suitreg_unregister(&entries[1]);
    TEST_ASSERT_NULL(suitreg_lookup_pid(TEST_UINT8));
}

void test_suitreg_unregister__success2(void)
{
    suitreg_t *res = NULL;

    TEST_ASSERT_EQUAL_INT(0, suitreg_register(&entries[1]));
    TEST_ASSERT_EQUAL_INT(0, suitreg_register(&entries[2]));
    suitreg_unregister(&entries[1]);
    TEST_ASSERT_NOT_NULL((res = suitreg_lookup_pid(TEST_UINT8+1)));
    TEST_ASSERT_EQUAL_INT(SUITREG_TYPE_ALL, res->type);
    suitreg_unregister(&entries[2]);
    TEST_ASSERT_NULL(suitreg_lookup_pid(TEST_UINT8+1));
}

void test_suitreg_unregister__success3(void)
{
    suitreg_t *res = NULL;

    TEST_ASSERT_EQUAL_INT(0, suitreg_register(&entries[1]));
    TEST_ASSERT_EQUAL_INT(0, suitreg_register(&entries[2]));
    suitreg_unregister(&entries[2]);
    TEST_ASSERT_NOT_NULL((res = suitreg_lookup_pid(TEST_UINT8)));
    TEST_ASSERT_EQUAL_INT(SUITREG_TYPE_ALL, res->type);
    suitreg_unregister(&entries[1]);
    TEST_ASSERT_NULL(suitreg_lookup_pid(TEST_UINT8));
}

Test *tests_suitreg_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_suitreg_register__inval),
        new_TestFixture(test_suitreg_register__success),
        new_TestFixture(test_suitreg_unregister__success),
        new_TestFixture(test_suitreg_unregister__success2),
        new_TestFixture(test_suitreg_unregister__success3),
    };

    EMB_UNIT_TESTCALLER(suitreg_tests, set_up, NULL, fixtures);

    return (Test *)&suitreg_tests;
}

void tests_suitreg(void)
{
    TESTS_RUN(tests_suitreg_tests());
}
/** @} */
