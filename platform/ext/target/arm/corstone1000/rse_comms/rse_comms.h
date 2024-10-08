/*
 * Copyright (c) 2022-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSE_COMMS_H__
#define __RSE_COMMS_H__

#include "psa/client.h"
#include "cmsis_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The size suits to fit the largest message too (EFI variables)
 * This size is defined by the Host's software stack.
 * The size was chosen by monitoring the messages that are coming
 * from the Trusted Services SE Proxy partition.
 */
#define RSE_COMMS_PAYLOAD_MAX_SIZE (0x43C0)

/*
 * Allocated for each client request.
 *
 * TODO: Sizing of payload_buf, this should be platform dependent:
 * - sum in_vec size
 * - sum out_vec size
 */
struct client_request_t {
    void *mhu_sender_dev; /* Pointer to MHU sender device to reply on */
    uint8_t protocol_ver;
    uint8_t seq_num;
    uint16_t client_id;
    psa_handle_t handle;
    int32_t type;
    uint32_t in_len;
    uint32_t out_len;
    psa_invec in_vec[PSA_MAX_IOVEC];
    psa_outvec out_vec[PSA_MAX_IOVEC];
    int32_t return_val;
    uint64_t out_vec_host_addr[PSA_MAX_IOVEC];
    uint8_t param_copy_buf[RSE_COMMS_PAYLOAD_MAX_SIZE];
};

#ifdef __cplusplus
}
#endif

#endif /* __RSE_COMMS_H__ */
