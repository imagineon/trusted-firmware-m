/*
 * Copyright (c) 2018-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "common_target_cfg.h"
#include "utilities.h"
/* "exception_info.h" must be the last include because of the IAR pragma */
#include "exception_info.h"

void C_MPC_Handler(void)
{
    /* Clear MPC interrupt flag and pending MPC IRQ */
    mpc_clear_irq();
    NVIC_ClearPendingIRQ(MPC_IRQn);

    /* Print fault message and block execution */
    ERROR_MSG("Platform Exception: MPC fault!!!");

    tfm_core_panic();
}

__attribute__((naked)) void MPC_Handler(void)
{
    EXCEPTION_INFO();

    __ASM volatile(
        "BL        C_MPC_Handler           \n"
        "B         .                       \n"
    );
}

void C_PPC_Handler(void)
{
    /* Clear PPC interrupt flag and pending PPC IRQ */
    ppc_clear_irq();
    NVIC_ClearPendingIRQ(PPC_IRQn);

    /* Print fault message*/
    ERROR_MSG("Platform Exception: PPC fault!!!");

    tfm_core_panic();
}

__attribute__((naked)) void PPC_Handler(void)
{
    EXCEPTION_INFO();

    __ASM volatile(
        "BL        C_PPC_Handler           \n"
        "B         .                       \n"
    );
}
