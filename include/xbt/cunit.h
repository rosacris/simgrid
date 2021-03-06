/* cunit - A little C Unit facility                                         */

/* Copyright (c) 2005, 2006, 2007, 2008, 2009, 2010. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

/* This is partially inspirated from the OSSP ts (Test Suite Library)       */

#ifndef _XBT_CUNIT_H_
#define _XBT_CUNIT_H_

#include "xbt/sysdep.h"         /* XBT_GNU_PRINTF */
#include "xbt/ex.h"

SG_BEGIN_DECL()

/* test suite object type */
typedef struct s_xbt_test_suite *xbt_test_suite_t;

/* test object type */
typedef struct s_xbt_test_unit *xbt_test_unit_t;

/* test callback function type */
typedef void (*ts_test_cb_t) (void);

/* test suite operations */
XBT_PUBLIC(xbt_test_suite_t) xbt_test_suite_new(const char *name,
                                                const char *fmt, ...);
XBT_PUBLIC(xbt_test_suite_t) xbt_test_suite_by_name(const char *name,
                                                    const char *fmt, ...);
XBT_PUBLIC(void) xbt_test_suite_dump(xbt_test_suite_t suite);
XBT_PUBLIC(void) xbt_test_suite_push(xbt_test_suite_t suite,
                                     const char *name, ts_test_cb_t func,
                                     const char *fmt, ...);

/* Run all the specified tests. what_to_do allows to disable some tests.
 * It is a coma (,) separated list of directives. They are applied from left to right.
 *
 * Each of them of form:
 * 
 * [-|+]suitename[:unitname[:testname]]
 * 
 * * First char: 
 *   if it's a '-', the directive disables something
 *   if it's a '+', the directive enables something
 *   By default, everything is enabled, but you can disable a suite and reenable some parts
 * * Suitename: the suite on which the directive acts
 * * unitname: if given, the unit on which the directive acts. If not, acts on any units.
 * * testname: if given, the test on which the directive acts. If not, acts on any tests.
 */

XBT_PUBLIC(int) xbt_test_run(char *selection);
/* Show information about the selection of tests */
XBT_PUBLIC(void) xbt_test_dump(char *selection);
/* Cleanup the mess */
XBT_PUBLIC(void) xbt_test_exit(void);

/* test operations */
XBT_PUBLIC(void) _xbt_test_add(const char *file, int line, const char *fmt,
                               ...) _XBT_GNUC_PRINTF(3, 4);
XBT_PUBLIC(void) _xbt_test_fail(const char *file, int line,
                                const char *fmt, ...) _XBT_GNUC_PRINTF(3,
                                                                       4);
XBT_PUBLIC(void) _xbt_test_log(const char *file, int line, const char *fmt,
                               ...) _XBT_GNUC_PRINTF(3, 4);

#define xbt_test_add(...)       _xbt_test_add(__FILE__, __LINE__, __VA_ARGS__)
#define xbt_test_fail(...)      _xbt_test_fail(__FILE__, __LINE__, __VA_ARGS__)
#define xbt_test_assert(...)    _XBT_IF_ONE_ARG(_xbt_test_assert_ARG1,  \
                                                _xbt_test_assert_ARGN,  \
                                                __VA_ARGS__)(__VA_ARGS__)
#define _xbt_test_assert_ARG1(cond)      _xbt_test_assert_CHECK(cond,   \
                                                                "%s", #cond)
#define _xbt_test_assert_ARGN(cond, ...) _xbt_test_assert_CHECK(cond,   \
                                                                __VA_ARGS__)
#define _xbt_test_assert_CHECK(cond, ...)                       \
  do { if (!(cond)) xbt_test_fail(__VA_ARGS__); } while (0)
#define xbt_test_log(...)       _xbt_test_log(__FILE__, __LINE__, __VA_ARGS__)

XBT_PUBLIC(void) xbt_test_exception(xbt_ex_t e);

XBT_PUBLIC(void) xbt_test_expect_failure(void);
XBT_PUBLIC(void) xbt_test_skip(void);

/* test suite short-cut macros */
#define XBT_TEST_UNIT(name,func,title)    \
    void func(void);  /*prototype*/       \
    void func(void)

#ifdef XBT_USE_DEPRECATED

/* Kept for backward compatibility. */

#define xbt_test_add0(...)      xbt_test_add(__VA_ARGS__)
#define xbt_test_add1(...)      xbt_test_add(__VA_ARGS__)
#define xbt_test_add2(...)      xbt_test_add(__VA_ARGS__)
#define xbt_test_add3(...)      xbt_test_add(__VA_ARGS__)
#define xbt_test_add4(...)      xbt_test_add(__VA_ARGS__)
#define xbt_test_add5(...)      xbt_test_add(__VA_ARGS__)

#define xbt_test_fail0(...)     xbt_test_fail(__VA_ARGS__)
#define xbt_test_fail1(...)     xbt_test_fail(__VA_ARGS__)
#define xbt_test_fail2(...)     xbt_test_fail(__VA_ARGS__)
#define xbt_test_fail3(...)     xbt_test_fail(__VA_ARGS__)
#define xbt_test_fail4(...)     xbt_test_fail(__VA_ARGS__)
#define xbt_test_fail5(...)     xbt_test_fail(__VA_ARGS__)

#define xbt_test_assert0(...)   xbt_test_assert(__VA_ARGS__)
#define xbt_test_assert1(...)   xbt_test_assert(__VA_ARGS__)
#define xbt_test_assert2(...)   xbt_test_assert(__VA_ARGS__)
#define xbt_test_assert3(...)   xbt_test_assert(__VA_ARGS__)
#define xbt_test_assert4(...)   xbt_test_assert(__VA_ARGS__)
#define xbt_test_assert5(...)   xbt_test_assert(__VA_ARGS__)

#define xbt_test_log0(...)      xbt_test_log(__VA_ARGS__)
#define xbt_test_log1(...)      xbt_test_log(__VA_ARGS__)
#define xbt_test_log2(...)      xbt_test_log(__VA_ARGS__)
#define xbt_test_log3(...)      xbt_test_log(__VA_ARGS__)
#define xbt_test_log4(...)      xbt_test_log(__VA_ARGS__)
#define xbt_test_log5(...)      xbt_test_log(__VA_ARGS__)

#endif

SG_END_DECL()
#endif                          /* _TS_H_ */
