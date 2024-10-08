#-------------------------------------------------------------------------------
# Copyright (c) 2022-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(TFM_BL2_ENCRYPTION_KEY_PATH         ${CMAKE_SOURCE_DIR}/bl1/bl1_2/bl2_dummy_encryption_key.bin CACHE STRING "Path to binary key to use for encrypting BL2")
set(TFM_GUK_PATH                        ${CMAKE_SOURCE_DIR}/bl1/bl1_1/dummy_guk.bin CACHE STRING "Path to binary GUK key file to provision")
set(TFM_BL2_IMAGE_FLASH_AREA_NUM        0           CACHE STRING    "Which flash area BL2 is stored in")

set(TFM_BL2_SIGNING_KEY_PATH            ${CMAKE_SOURCE_DIR}/bl1/bl1_2/bl1_dummy_rotpk CACHE FILEPATH "Path to binary BL2 signing private key")

set(TFM_BL1_MEMORY_MAPPED_FLASH         ON          CACHE BOOL      "Whether BL1 can directly access flash content")

set(TFM_BL1_LOG_LEVEL                   LOG_LEVEL_INFO CACHE STRING "The level of BL1 logging to uart")
set(TFM_BL1_DEFAULT_OTP                 ON          CACHE BOOL      "Whether BL1_1 will use default OTP memory")
set(TFM_BL1_DEFAULT_PROVISIONING        ON          CACHE BOOL      "Whether BL1_1 will use default provisioning")
set(TFM_BL1_SOFTWARE_CRYPTO             ON          CACHE BOOL      "Whether BL1_1 will use software crypto")
set(TFM_BL1_DUMMY_TRNG                  ON          CACHE BOOL      "Whether BL1_1 will use dummy TRNG")
set(TFM_BL1_PQ_CRYPTO                   OFF         CACHE BOOL      "Enable LMS PQ crypto for BL2 verification. This is experimental and should not yet be used in production")

set(TFM_BL1_IMAGE_VERSION_BL2           "1.9.0+0"   CACHE STRING    "Image version of BL2 image")
set(TFM_BL1_IMAGE_SECURITY_COUNTER_BL2  1           CACHE STRING    "Security counter value to include with BL2 image")

set(TFM_BL1_2_IN_OTP                    TRUE        CACHE BOOL      "Whether BL1_2 is stored in OTP")
set(TFM_BL1_2_IN_FLASH                  FALSE       CACHE BOOL      "Whether BL1_2 is stored in FLASH")

set(BL1_HEADER_SIZE                     0x800       CACHE STRING    "BL1 Header size")
set(BL1_TRAILER_SIZE                    0x000       CACHE STRING    "BL1 Trailer size")

set(BL1_2_BUILD_TYPE                    ${CMAKE_BUILD_TYPE} CACHE STRING "Build type of BL1_2")

set(BL1_1_SHARED_SYMBOLS_PATH ${CMAKE_SOURCE_DIR}/bl1/bl1_1/bl1_1_shared_symbols.txt CACHE FILEPATH "Path to list of symbols that BL1_1 that can be referenced from BL1_2")

set(PLATFORM_DEFAULT_BL1_TEST_EXECUTION ON         CACHE BOOL "Whether to use platform-specific mechanism to run BL1 tests")
set(PLATFORM_DEFAULT_BL1_1_TESTS        ON         CACHE BOOL "Whether to use platform-specific BL1_1 testsuite")
set(PLATFORM_DEFAULT_BL1_2_TESTS        ON         CACHE BOOL "Whether to use platform-specific BL1_2 testsuite")
set(EXTRA_BL1_1_TEST_SUITE_PATH         ""         CACHE STRING "path to extra BL1_1 testsuite")
set(EXTRA_BL1_2_TEST_SUITE_PATH         ""         CACHE STRING "path to extra BL1_2 testsuite")
