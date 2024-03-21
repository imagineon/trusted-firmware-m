/*
 * Copyright (c) 2022-2024 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file device_definition.h
 * \brief The structure definitions in this file are exported based on the
 * peripheral definitions from device_cfg.h.
 * This file is meant to be used as a helper for baremetal
 * applications and/or as an example of how to configure the generic
 * driver structures.
 */

#ifndef __HOST_DEVICE_DEFINITION_H__
#define __HOST_DEVICE_DEFINITION_H__

#include "host_device_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/* UART PL011 driver structures */
#ifdef UART0_PL011_S
#include "uart_pl011_drv.h"
extern struct uart_pl011_dev_t UART0_PL011_DEV_S;
#endif
#ifdef UART0_PL011_NS
#include "uart_pl011_drv.h"
extern struct uart_pl011_dev_t UART0_PL011_DEV_NS;
#endif
#ifdef UART0_LOCAL_PL011_S
#include "uart_pl011_drv.h"
extern struct uart_pl011_dev_t UART0_LOCAL_PL011_DEV_S;
#endif
#ifdef UART0_LOCAL_PL011_NS
#include "uart_pl011_drv.h"
extern struct uart_pl011_dev_t UART0_LOCAL_PL011_DEV_NS;
#endif

#if (defined(SPI_STRATAFLASHJ3_S) && defined(CFI_S))
#include "spi_strataflashj3_flash_lib.h"
extern struct cfi_strataflashj3_dev_t SPI_STRATAFLASHJ3_DEV;
#endif

#if PLAT_MHU_VERSION == 2
#include "mhu_v2_x.h"
#define MHU_STRUCT_TYPE struct mhu_v2_x_dev_t
#elif PLAT_MHU_VERSION == 3
#include "mhu_v3_x.h"
#define MHU_PBX_DBCH_FLAG_SCP_COMMS     UINT32_C(0x1)
#define MHU_STRUCT_TYPE struct mhu_v3_x_dev_t
#else
#error Invalid MHU version
#endif /* PLAT_MHU_VERSION == 2 */

/* Message Handling Units (MHU) */
#ifdef MHU_AP_MONITOR_TO_RSE
extern MHU_STRUCT_TYPE MHU_AP_MONITOR_TO_RSE_DEV;
#endif

#ifdef MHU_RSE_TO_AP_MONITOR
extern MHU_STRUCT_TYPE MHU_RSE_TO_AP_MONITOR_DEV;
#endif

#ifdef MHU_AP_NS_TO_RSE
extern MHU_STRUCT_TYPE MHU_AP_NS_TO_RSE_DEV;
#endif

#ifdef MHU_RSE_TO_AP_NS
extern MHU_STRUCT_TYPE MHU_RSE_TO_AP_NS_DEV;
#endif

#ifdef MHU_SCP_TO_RSE
extern MHU_STRUCT_TYPE MHU_SCP_TO_RSE_DEV;
#endif

#ifdef MHU_RSE_TO_SCP
extern MHU_STRUCT_TYPE MHU_RSE_TO_SCP_DEV;
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __DEVICE_DEFINITION_H__ */
