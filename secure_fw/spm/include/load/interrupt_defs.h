/*
 * Copyright (c) 2021-2024, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __INTERRUPT_DEFS_H__
#define __INTERRUPT_DEFS_H__

#include <stdint.h>
#include "tfm_hal_defs.h"
#include "psa/service.h"

/* IRQ static load info */
struct irq_load_info_t {
    /*
     * Initialization function per interrupt.
     * args:  owner Partition, irq_load_info
     */
    enum tfm_hal_status_t (*init)(void *pt, const struct irq_load_info_t *pildi);
    psa_flih_result_t (*flih_func)(void);     /* FLIH function, NULL for SLIH */
    int32_t      pid;                         /* Owner Partition ID           */
    uint32_t     source;                      /* IRQ source (number/index)    */
    psa_signal_t signal;                      /* The signal assigned for IRQ  */
    int32_t     client_id_base;               /* The min translated client ID */
    int32_t     client_id_limit;              /* The max translated client ID */
};

/* IRQ runtime data */
struct irq_t {
    struct partition_t           *p_pt;       /* The owner Partition     */
    const struct irq_load_info_t *p_ildi;     /* The interrupt load info */
};

#endif /* __INTERRUPT_DEFS_H__ */
