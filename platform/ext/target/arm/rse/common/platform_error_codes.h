/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_ERROR_CODES_H__
#define __PLATFORM_ERROR_CODES_H__

#include <limits.h>

#ifdef TFM_UNIQUE_ERROR_CODES
#include "error_codes_mapping.h"
#else
#define TFM_PLAT_ERROR_BASE 0x1u
#endif /* TFM_UNIQUE_ERROR_CODES */

enum tfm_plat_err_t {
    TFM_PLAT_ERR_SUCCESS = 0,
    /* RSE OTP LCM error codes */
    TFM_PLAT_ERR_OTP_EMULATION_UNSUPPORTED = TFM_PLAT_ERROR_BASE,
    TFM_PLAT_ERR_OTP_READ_UNSUPPORTED,
    TFM_PLAT_ERR_OTP_READ_ENCRYPTED_UNSUPPORTED,
    TFM_PLAT_ERR_OTP_READ_ENCRYPTED_INVALID_INPUT,
    TFM_PLAT_ERR_OTP_WRITE_INVALID_INPUT,
    TFM_PLAT_ERR_OTP_WRITE_ENCRYPTED_INVALID_INPUT,
    TFM_PLAT_ERR_OTP_READ_LCS_INVALID_INPUT,
    TFM_PLAT_ERR_OTP_WRITE_LCS_INVALID_INPUT,
    TFM_PLAT_ERR_OTP_WRITE_LCS_SYSTEM_ERR,
    TFM_PLAT_ERR_OTP_INIT_SYSTEM_ERR,
    TFM_PLAT_ERR_PLAT_OTP_READ_INVALID_INPUT,
    TFM_PLAT_ERR_PLAT_OTP_READ_UNSUPPORTED,
    TFM_PLAT_ERR_PLAT_OTP_READ_MFG_DATA_UNSUPPORTED,
    TFM_PLAT_ERR_PLAT_OTP_WRITE_INVALID_INPUT,
    TFM_PLAT_ERR_PLAT_OTP_WRITE_UNSUPPORTED,
    TFM_PLAT_ERR_PLAT_OTP_GET_SIZE_INVALID_INPUT,
    TFM_PLAT_ERR_PLAT_OTP_GET_SIZE_UNSUPPORTED,
    /* RSE BL1 provisioning error codes */
    TFM_PLAT_ERR_BL1_PROVISIONING_INVALID_TP_MODE,
    /* RSE key derivation error codes */
    TFM_PLAT_ERR_KEY_DERIVATION_BOOT_STATE_BUFFER_TOO_SMALL,
    TFM_PLAT_ERR_KEY_DERIVATION_INVALID_TP_MODE,
    /* RSE NV counters */
    TFM_PLAT_ERR_READ_OTP_COUNTER_SYSTEM_ERR,
    TFM_PLAT_ERR_READ_NV_COUNTER_INVALID_COUNTER_SIZE,
    TFM_PLAT_ERR_READ_NV_COUNTER_UNSUPPORTED,
    TFM_PLAT_ERR_SET_OTP_COUNTER_MAX_VALUE,
    TFM_PLAT_ERR_SET_NV_COUNTER_UNSUPPORTED,
    TFM_PLAT_ERR_ICREMENT_NV_COUNTER_MAX_VALUE,
    /* Generic errors */
    TFM_PLAT_ERR_SYSTEM_ERR,
    TFM_PLAT_ERR_MAX_VALUE,
    TFM_PLAT_ERR_INVALID_INPUT,
    TFM_PLAT_ERR_UNSUPPORTED,
    TFM_PLAT_ERR_NOT_PERMITTED,
    /* Following entry is only to ensure the error code of int size */
    TFM_PLAT_ERR_FORCE_UINT_SIZE = UINT_MAX
};

#endif /* __PLATFORM_ERROR_CODES_H__ */
