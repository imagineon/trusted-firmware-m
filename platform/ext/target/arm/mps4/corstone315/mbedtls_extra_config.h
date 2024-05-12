/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#undef PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE   1

#undef PSA_WANT_ECC_SECP_R1_256
#define PSA_WANT_ECC_SECP_R1_256                1

#undef PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY
#define PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY        1

#undef PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC    1

#undef PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT   1

#undef PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT   1

#undef PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1

#undef PSA_WANT_ALG_HKDF
#define PSA_WANT_ALG_HKDF                       1

#undef MBEDTLS_PSA_P256M_DRIVER_ENABLED
#define MBEDTLS_PSA_P256M_DRIVER_ENABLED
