/*
 * Copyright (c) 2020-2024, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <arm_cmse.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "array.h"
#include "tfm_hal_device_header.h"
#include "Driver_Common.h"
#include "mmio_defs.h"
#include "mpu_armv8m_drv.h"
#include "region.h"
#include "target_cfg.h"
#include "tfm_hal_defs.h"
#include "tfm_hal_isolation.h"
#include "tfm_plat_defs.h"
#include "tfm_peripherals_def.h"
#include "load/partition_defs.h"
#include "load/asset_defs.h"
#include "load/spm_load_api.h"
#include "low_level_rng.h"

/* It can be retrieved from the MPU_TYPE register. */
#define MPU_REGION_NUM                  8

#if TFM_ISOLATION_LEVEL == 3
#define PROT_BOUNDARY_VAL \
    (((1U << HANDLE_ATTR_PRIV_POS) & HANDLE_ATTR_PRIV_MASK) | \
     ((1U << HANDLE_ATTR_SPM_POS) & HANDLE_ATTR_SPM_MASK))
#else
#define PROT_BOUNDARY_VAL \
    ((1U << HANDLE_ATTR_PRIV_POS) & HANDLE_ATTR_PRIV_MASK)
#endif

#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT
static uint32_t n_configured_regions = 0;
struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };

#if TFM_ISOLATION_LEVEL == 3
static uint32_t idx_boundary_handle = 0;
#endif /* TFM_ISOLATION_LEVEL == 3 */

REGION_DECLARE(Image$$, PT_UNPRIV_CODE_START, $$Base);
REGION_DECLARE(Image$$, PT_APP_ROT_CODE_END, $$Base);
REGION_DECLARE(Image$$, PT_PSA_ROT_CODE_START, $$Base);
REGION_DECLARE(Image$$, PT_PSA_ROT_CODE_END, $$Base);
REGION_DECLARE(Image$$, PT_RO_DATA_START, $$Base);
REGION_DECLARE(Image$$, PT_RO_DATA_END, $$Base);
#if TFM_ISOLATION_LEVEL != 3
REGION_DECLARE(Image$$, PT_APP_ROT_DATA_START, $$Base);
REGION_DECLARE(Image$$, PT_APP_ROT_DATA_END, $$Base);
#elif defined(CONFIG_TFM_PARTITION_META)
REGION_DECLARE(Image$$, TFM_SP_META_PTR, $$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SP_META_PTR_END, $$ZI$$Limit);
#endif
REGION_DECLARE(Image$$, PT_PSA_ROT_DATA_START, $$Base);
REGION_DECLARE(Image$$, PT_PSA_ROT_DATA_END, $$Base);

static const struct mpu_armv8m_region_cfg_t region_cfg[] = {
    /* Combined unprivileged/ARoT code region */
    {
        0, /* will be updated before using */
        (uint32_t)&REGION_NAME(Image$$, PT_UNPRIV_CODE_START, $$Base),
        (uint32_t)&REGION_NAME(Image$$, PT_APP_ROT_CODE_END, $$Base),
        MPU_ARMV8M_MAIR_ATTR_CODE_IDX,
        MPU_ARMV8M_XN_EXEC_OK,
        MPU_ARMV8M_AP_RO_PRIV_UNPRIV,
        MPU_ARMV8M_SH_NONE
    },
    /* PSA RoT code region */
    {
        0, /* will be updated before using */
        (uint32_t)&REGION_NAME(Image$$, PT_PSA_ROT_CODE_START, $$Base),
        (uint32_t)&REGION_NAME(Image$$, PT_PSA_ROT_CODE_END, $$Base),
        MPU_ARMV8M_MAIR_ATTR_CODE_IDX,
        MPU_ARMV8M_XN_EXEC_OK,
        MPU_ARMV8M_AP_RO_PRIV_ONLY,
        MPU_ARMV8M_SH_NONE
    },
    /* RO data region */
    {
        0, /* will be updated before using */
        (uint32_t)&REGION_NAME(Image$$, PT_RO_DATA_START, $$Base),
        (uint32_t)&REGION_NAME(Image$$, PT_RO_DATA_END, $$Base),
        MPU_ARMV8M_MAIR_ATTR_CODE_IDX,
        MPU_ARMV8M_XN_EXEC_NEVER,
        MPU_ARMV8M_AP_RO_PRIV_UNPRIV,
        MPU_ARMV8M_SH_NONE
    },
    /* For isolation levels below 3, configure all App RoT data (which includes
     * the metadata pointer region) as unprivileged. For isolation level 3, only
     * the metadata pointer region is configured unprivileged and App RoT
     * partition data regions are dynamically configured during partition
     * scheduling.
     */
#if TFM_ISOLATION_LEVEL != 3
    /* Application RoT data region */
    {
        0, /* will be updated before using */
        (uint32_t)&REGION_NAME(Image$$, PT_APP_ROT_DATA_START, $$Base),
        (uint32_t)&REGION_NAME(Image$$, PT_APP_ROT_DATA_END, $$Base),
        MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
        MPU_ARMV8M_XN_EXEC_NEVER,
        MPU_ARMV8M_AP_RW_PRIV_UNPRIV,
        MPU_ARMV8M_SH_NONE
    },
#elif defined(CONFIG_TFM_PARTITION_META)
    /* TFM partition metadata pointer region */
    {
        0, /* will be updated before using */
        (uint32_t)&REGION_NAME(Image$$, TFM_SP_META_PTR, $$ZI$$Base),
        (uint32_t)&REGION_NAME(Image$$, TFM_SP_META_PTR_END, $$ZI$$Limit),
        MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
        MPU_ARMV8M_XN_EXEC_NEVER,
        MPU_ARMV8M_AP_RW_PRIV_UNPRIV,
        MPU_ARMV8M_SH_NONE
    },
#endif
    /* PSA RoT data region */
    {
        0, /* will be updated before using */
        (uint32_t)&REGION_NAME(Image$$, PT_PSA_ROT_DATA_START, $$Base),
        (uint32_t)&REGION_NAME(Image$$, PT_PSA_ROT_DATA_END, $$Base),
        MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
        MPU_ARMV8M_XN_EXEC_NEVER,
        MPU_ARMV8M_AP_RW_PRIV_ONLY,
        MPU_ARMV8M_SH_NONE
    },
};
#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

enum tfm_hal_status_t tfm_hal_set_up_static_boundaries(
                                            uintptr_t *p_spm_boundary)
{
    /* Set up isolation boundaries between SPE and NSPE */
    gtzc_init_cfg();
    sau_and_idau_cfg();
    pinmux_init_cfg();
    /* Start HW randomization */
    if (RNG_Init()) {
        return TFM_HAL_ERROR_GENERIC;
    }
    /* Set up static isolation boundaries inside SPE */
#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT
    struct mpu_armv8m_region_cfg_t localcfg;
    int32_t i;

    mpu_armv8m_clean(&dev_mpu_s);

#if TFM_ISOLATION_LEVEL == 3
    /*
     * Update MPU region numbers. The numbers start from 0 and are continuous.
     * Under isolation level 3, at least one MPU region is reserved for private
     * data asset.
     */
    if (ARRAY_SIZE(region_cfg) >= MPU_REGION_NUM) {
        return TFM_HAL_ERROR_GENERIC;
    }
#else /* TFM_ISOLATION_LEVEL == 3 */
    if (ARRAY_SIZE(region_cfg) > MPU_REGION_NUM) {
        return TFM_HAL_ERROR_GENERIC;
    }
#endif /* TFM_ISOLATION_LEVEL == 3 */
    for (i = 0; i < ARRAY_SIZE(region_cfg); i++) {
        memcpy(&localcfg, &region_cfg[i], sizeof(localcfg));
        /* Update region number */
        localcfg.region_nr = i;
        /* Enable region */
        if (mpu_armv8m_region_enable(&dev_mpu_s, &localcfg) != MPU_ARMV8M_OK) {
            return TFM_HAL_ERROR_GENERIC;
        }
    }
    n_configured_regions = i;

    /* Enable MPU */
    if (mpu_armv8m_enable(&dev_mpu_s,
                          PRIVILEGED_DEFAULT_ENABLE,
                          HARDFAULT_NMI_ENABLE) != MPU_ARMV8M_OK) {
        return TFM_HAL_ERROR_GENERIC;
    }
#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

    *p_spm_boundary = (uintptr_t)PROT_BOUNDARY_VAL;

    return TFM_HAL_SUCCESS;
}

/*
 * Implementation of tfm_hal_bind_boundary() on STM:
 *
 * The API encodes some attributes into a handle and returns it to SPM.
 * The attributes include isolation boundaries, privilege, and MMIO information.
 * When scheduler switches running partitions, SPM compares the handle between
 * partitions to know if boundary update is necessary. If update is required,
 * SPM passes the handle to platform to do platform settings and update
 * isolation boundaries.
 *
 * The handle should be unique under isolation level 3. The implementation
 * encodes an index at the highest 8 bits to assure handle uniqueness. While
 * under isolation level 1/2, handles may not be unique.
 *
 * The encoding format assignment:
 * - For isolation level 3
 *      BIT | 31        24 | 23         20 | ... | 7           4 | 3       0 |
 *          | Unique Index | Region Attr 5 | ... | Region Attr 1 | Base Attr |
 *
 *      In which the "Region Attr i" is:
 *      BIT |       3      | 2        0 |
 *          | 1: RW, 0: RO | MMIO Index |
 *
 *      In which the "Base Attr" is:
 *      BIT |               1                |                           0                     |
 *          | 1: privileged, 0: unprivileged | 1: Trustzone-specific NSPE, 0: Secure partition |
 *
 * - For isolation level 1/2
 *      BIT | 31     2 |              1                |                           0                     |
 *          | Reserved |1: privileged, 0: unprivileged | 1: Trustzone-specific NSPE, 0: Secure partition |
 *
 * This is a reference implementation on STM, and may have some limitations.
 * 1. The maximum number of allowed MMIO regions is 5.
 * 2. Highest 8 bits are for index. It supports 256 unique handles at most.
 */

enum tfm_hal_status_t tfm_hal_bind_boundary(
                                    const struct partition_load_info_t *p_ldinf,
                                    uintptr_t *p_boundary)
{
    uint32_t i, j;
    bool privileged;
    bool ns_agent;
    uint32_t partition_attrs = 0;
    const struct asset_desc_t *p_asset;
#if TFM_ISOLATION_LEVEL == 2
    struct platform_data_t *plat_data_ptr;
    struct mpu_armv8m_region_cfg_t localcfg;
#endif

    if (!p_ldinf || !p_boundary) {
        return TFM_HAL_ERROR_GENERIC;
    }

#if TFM_ISOLATION_LEVEL == 1
    privileged = true;
#else
    privileged = IS_PSA_ROT(p_ldinf);
#endif

    ns_agent = IS_NS_AGENT(p_ldinf);
    p_asset = LOAD_INFO_ASSET(p_ldinf);

    /*
     * Validate if the named MMIO of partition is allowed by the platform.
     * Otherwise, skip validation.
     *
     * NOTE: Need to add validation of numbered MMIO if platform requires.
     */
    for (i = 0; i < p_ldinf->nassets; i++) {
        if (!(p_asset[i].attr & ASSET_ATTR_NAMED_MMIO)) {
            continue;
        }
        for (j = 0; j < ARRAY_SIZE(partition_named_mmio_list); j++) {
            if (p_asset[i].dev.dev_ref == partition_named_mmio_list[j]) {
                break;
            }
        }

        if (j == ARRAY_SIZE(partition_named_mmio_list)) {
            /* The MMIO asset is not in the allowed list of platform. */
            return TFM_HAL_ERROR_GENERIC;
        }
#if TFM_ISOLATION_LEVEL == 2
        plat_data_ptr = REFERENCE_TO_PTR(p_asset[i].dev.dev_ref,
                                         struct platform_data_t *);
        /*
         * Static boundaries are set. Set up MPU region for MMIO.
         * Setup regions for unprivileged assets only.
         */
        if (!privileged) {
            localcfg.region_base = plat_data_ptr->periph_start;
            localcfg.region_limit = plat_data_ptr->periph_limit;
            localcfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DEVICE_IDX;
            localcfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
            localcfg.attr_sh = MPU_ARMV8M_SH_NONE;
            localcfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
            localcfg.region_nr = n_configured_regions++;

            if (mpu_armv8m_region_enable(&dev_mpu_s, &localcfg)
                != MPU_ARMV8M_OK) {
                return TFM_HAL_ERROR_GENERIC;
            }
        }
#elif TFM_ISOLATION_LEVEL == 3
        /* Encode MMIO attributes into the "partition_attrs". */
        partition_attrs <<= HANDLE_PER_ATTR_BITS;
        partition_attrs |= ((j + 1) & HANDLE_ATTR_INDEX_MASK);
        if (p_asset[i].attr & ASSET_ATTR_READ_WRITE) {
            partition_attrs |= HANDLE_ATTR_RW_POS;
        }
#endif
    }

#if TFM_ISOLATION_LEVEL == 3
    partition_attrs <<= HANDLE_PER_ATTR_BITS;
    /*
     * Highest 8 bits are reserved for index, if they are non-zero, MMIO numbers
     * must have exceeded the limit of 5.
     */
    if (partition_attrs & HANDLE_INDEX_MASK) {
        return TFM_HAL_ERROR_GENERIC;
    }
    HANDLE_ENCODE_INDEX(partition_attrs, idx_boundary_handle);
#endif

    partition_attrs |= ((uint32_t)privileged << HANDLE_ATTR_PRIV_POS) &
                        HANDLE_ATTR_PRIV_MASK;
    partition_attrs |= ((uint32_t)ns_agent << HANDLE_ATTR_NS_POS) &
                        HANDLE_ATTR_NS_MASK;
    *p_boundary = (uintptr_t)partition_attrs;

    return TFM_HAL_SUCCESS;
}

enum tfm_hal_status_t tfm_hal_activate_boundary(
                             const struct partition_load_info_t *p_ldinf,
                             uintptr_t boundary)
{
    CONTROL_Type ctrl;
    uint32_t local_handle = (uint32_t)boundary;
    bool privileged = !!(local_handle & HANDLE_ATTR_PRIV_MASK);
#if TFM_ISOLATION_LEVEL == 3
    bool is_spm = !!(local_handle & HANDLE_ATTR_SPM_MASK);
    struct mpu_armv8m_region_cfg_t localcfg;
    uint32_t i, mmio_index;
    struct platform_data_t *plat_data_ptr;
    const struct asset_desc_t *rt_mem;
#endif

    /* Privileged level is required to be set always */
    ctrl.w = __get_CONTROL();
    ctrl.b.nPRIV = privileged ? 0 : 1;
    __set_CONTROL(ctrl.w);

#if TFM_ISOLATION_LEVEL == 3
    if (is_spm) {
        FIH_RET(fih_int_encode(TFM_HAL_SUCCESS));
    }

    if (!p_ldinf) {
        return TFM_HAL_ERROR_GENERIC;
    }

    /* Update regions, for unprivileged partitions only */
    if (privileged) {
        return TFM_HAL_SUCCESS;
    }

    /* Setup runtime memory first */
    localcfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
    localcfg.attr_sh = MPU_ARMV8M_SH_NONE;
    localcfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
    localcfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
    rt_mem = LOAD_INFO_ASSET(p_ldinf);
    /*
     * STM shortcut: The first item is the only runtime memory asset.
     * Platforms with many memory assets please check this part.
     */
    for (i = 0;
         i < p_ldinf->nassets && !(rt_mem[i].attr & ASSET_ATTR_MMIO);
         i++) {
        localcfg.region_nr = n_configured_regions + i;
        localcfg.region_base = rt_mem[i].mem.start;
        localcfg.region_limit = rt_mem[i].mem.limit;

        if (mpu_armv8m_region_enable(&dev_mpu_s, &localcfg) != MPU_ARMV8M_OK) {
            return TFM_HAL_ERROR_GENERIC;
        }
    }

    /* Named MMIO part */
    local_handle = local_handle & (~HANDLE_INDEX_MASK);
    local_handle >>= HANDLE_PER_ATTR_BITS;
    mmio_index = local_handle & HANDLE_ATTR_INDEX_MASK;

    localcfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DEVICE_IDX;

    i = n_configured_regions + i;
    while (mmio_index && i < MPU_REGION_NUM) {
        plat_data_ptr =
          (struct platform_data_t *)partition_named_mmio_list[mmio_index - 1];
        localcfg.region_nr = i++;
        localcfg.attr_access = (local_handle & HANDLE_ATTR_RW_POS)?
                            MPU_ARMV8M_AP_RW_PRIV_UNPRIV :
                            MPU_ARMV8M_AP_RO_PRIV_UNPRIV;
        localcfg.region_base = plat_data_ptr->periph_start;
        localcfg.region_limit = plat_data_ptr->periph_limit;

        if (mpu_armv8m_region_enable(&dev_mpu_s, &localcfg) != MPU_ARMV8M_OK) {
            return TFM_HAL_ERROR_GENERIC;
        }

        local_handle >>= HANDLE_PER_ATTR_BITS;
        mmio_index = local_handle & HANDLE_ATTR_INDEX_MASK;
    }

    /* Disable unused regions */
    while (i < MPU_REGION_NUM) {
        if (mpu_armv8m_region_disable(&dev_mpu_s, i++)!= MPU_ARMV8M_OK) {
            return TFM_HAL_ERROR_GENERIC;
        }
    }
#endif
    return TFM_HAL_SUCCESS;
}

enum tfm_hal_status_t tfm_hal_memory_check(uintptr_t boundary, uintptr_t base,
                                           size_t size, uint32_t access_type)
{
    int flags = 0;

    /* If size is zero, this indicates an empty buffer and base is ignored */
    if (size == 0) {
        return TFM_HAL_SUCCESS;
    }

    if (!base) {
        return TFM_HAL_ERROR_INVALID_INPUT;
    }

    if ((access_type & TFM_HAL_ACCESS_READWRITE) == TFM_HAL_ACCESS_READWRITE) {
        flags |= CMSE_MPU_READWRITE;
    } else if (access_type & TFM_HAL_ACCESS_READABLE) {
        flags |= CMSE_MPU_READ;
    } else {
        return TFM_HAL_ERROR_INVALID_INPUT;
    }

    if (!((uint32_t)boundary & HANDLE_ATTR_PRIV_MASK)) {
        flags |= CMSE_MPU_UNPRIV;
    }

    if ((uint32_t)boundary & HANDLE_ATTR_NS_MASK) {
        CONTROL_Type ctrl;
        ctrl.w = __TZ_get_CONTROL_NS();
        if (ctrl.b.nPRIV == 1) {
            flags |= CMSE_MPU_UNPRIV;
        } else {
            flags &= ~CMSE_MPU_UNPRIV;
        }
        flags |= CMSE_NONSECURE;
    }

    if (cmse_check_address_range((void *)base, size, flags) != NULL) {
        return TFM_HAL_SUCCESS;
    } else {
        return TFM_HAL_ERROR_MEM_FAULT;
    }
}

bool tfm_hal_boundary_need_switch(uintptr_t boundary_from,
                                  uintptr_t boundary_to)
{
    if (boundary_from == boundary_to) {
        return false;
    }

    if (((uint32_t)boundary_from & HANDLE_ATTR_PRIV_MASK) &&
        ((uint32_t)boundary_to & HANDLE_ATTR_PRIV_MASK)) {
        return false;
    }
    return true;
}
