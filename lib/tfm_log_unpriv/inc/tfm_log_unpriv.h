/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TF_M_LOG_UNPRIV_H__
#define __TF_M_LOG_UNPRIV_H__

#include <stdbool.h>
#include <stdint.h>

#include "tfm_vprintf.h"

/*
 * If the log output is too low then this macro is used in place of tfm_log_unpriv()
 * below. The intent is to get the compiler to evaluate the function call for
 * type checking and format specifier correctness but let it optimize it out.
 */
#define no_tfm_log_unpriv(fmt, ...)                \
    do {                                         \
        if (false) {                             \
            tfm_log_unpriv(fmt, ##__VA_ARGS__);    \
        }                                        \
    } while (false)

#ifndef LOG_LEVEL_UNPRIV
#error "LOG_LEVEL_UNPRIV not defined!"
#endif

#if LOG_LEVEL_UNPRIV >= LOG_LEVEL_ERROR
# define ERROR_UNPRIV(...)    tfm_log_unpriv(LOG_MARKER_ERROR __VA_ARGS__)
#else
# define ERROR_UNPRIV(...)    no_tfm_log_unpriv(LOG_MARKER_ERROR __VA_ARGS__)
#endif

#if LOG_LEVEL_UNPRIV >= LOG_LEVEL_NOTICE
# define NOTICE_UNPRIV(...)    tfm_log_unpriv(LOG_MARKER_NOTICE __VA_ARGS__)
#else
# define NOTICE_UNPRIV(...)    no_tfm_log_unpriv(LOG_MARKER_NOTICE __VA_ARGS__)
#endif

#if LOG_LEVEL_UNPRIV >= LOG_LEVEL_WARNING
# define WARN_UNPRIV(...)    tfm_log_unpriv(LOG_MARKER_WARNING __VA_ARGS__)
#else
# define WARN_UNPRIV(...)    no_tfm_log_unpriv(LOG_MARKER_WARNING __VA_ARGS__)
#endif

#if LOG_LEVEL_UNPRIV >= LOG_LEVEL_INFO
# define INFO_UNPRIV(...)    tfm_log_unpriv(LOG_MARKER_INFO __VA_ARGS__)
#else
# define INFO_UNPRIV(...)    no_tfm_log_unpriv(LOG_MARKER_INFO __VA_ARGS__)
#endif

#if LOG_LEVEL_UNPRIV >= LOG_LEVEL_VERBOSE
# define VERBOSE_UNPRIV(...)    tfm_log_unpriv(LOG_MARKER_VERBOSE __VA_ARGS__)
#else
# define VERBOSE_UNPRIV(...)    no_tfm_log_unpriv(LOG_MARKER_VERBOSE __VA_ARGS__)
#endif

#if defined(__ICCARM__)
#pragma __printf_args
void tfm_log_unpriv(const char *fmt, ...);
#else
__attribute__((format(printf, 1, 2)))
void tfm_log_unpriv(const char *fmt, ...);
#endif

#endif /* __TF_M_LOG_UNPRIV_H__ */
