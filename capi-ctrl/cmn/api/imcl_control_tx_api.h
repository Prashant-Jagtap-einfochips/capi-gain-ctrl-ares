#ifndef IMCL_AVC_API_H
#define IMCL_AVC_API_H

/**
  @file imcl_avc_api.h

  @brief defines the Intent IDs for communication over Inter-Module Control
  Links (IMCL) between SP Tx and Rx Modules

*/
/*==========================================================================
 * Copyright (c) 2019-2021 Qualcomm Technologies Incorporated
 * All rights reserved
 * Qualcomm Technologies. Proprietary and Confidential
 * =========================================================================*/
/*===========================================================================
  Edit History

  when          who         what, where, why
  --------    -------      --------------------------------------------------

=============================================================================*/
#include "ar_defs.h"

#define INTENT_ID_GAIN_CONTROL 0x18001231

#ifdef INTENT_ID_GAIN_CONTROL

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/**< Header - Any IMCL message between soft volume module and Popless equalizer
 *  will have the following header followed by the actual payload.
 *  The peers have to parse the header accordingly*/

typedef struct gain_imcl_header_t
{
   // specific purpose understandable to the IMCL peers only
   uint32_t opcode;

   // Size (in bytes) for the payload specific to the intent.
   uint32_t actual_data_len;

} gain_imcl_header_t;

#define MIN_INCOMING_IMCL_PARAM_SIZE_GAIN (sizeof(gain_imcl_header_t) + sizeof(intf_extn_param_id_imcl_incoming_data_t))

/* ============================================================================
   Param ID
==============================================================================*/

#define PARAM_ID_GAIN_CONTROL_IMC_PAYLOAD 0x18001232

/*==============================================================================
   Param structure defintions
==============================================================================*/

typedef struct capi_gain_control_data_payload_t
{

   unsigned int gain;
   /**< @h2xmle_description {Controls audio decimation}
        @h2xmle_default     {0x0000}
        @h2xmle_range       {0..0xFFFFFFFF}

        @h2xmle_policy      {Basic} */
} capi_gain_control_data_payload_t;

#define PARAM_ID_GAIN_COEFF_IMC_PAYLOAD 0x18001233

typedef struct capi_gain_coeff_arr_payload_t
{

   uint8_t coeff_val[4096];
   /**< @h2xmle_description {Coefficient values}
        @h2xmle_default     {0x00}
        @h2xmle_range       {0..0xFF}
        
        @h2xmle_policy      {Basic} */
} capi_gain_coeff_arr_payload_t;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // INTENT_ID_AVC

#endif /* IMCL_AVC_API_H */
