#ifndef _GAIN_MODULE_API_H_
#define _GAIN_MODULE_API_H_

/*==============================================================================
  @file control_tx_module_api.h
  @brief This file contains Gain Module APIs

  Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/
// clang-format off
/* =========================================================================
   Edit History

 ======================================================================== */
// clang-format on

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#include "module_cmn_api.h"
#include "imcl_control_tx_api.h"

/** @h2xml_title1          {Gain Control Module API}
    @h2xml_title_agile_rev {Gain Control Module API}
    @h2xml_title_date      {Oct 03, 2019} */

/*------------------------------------------------------------------------------
   Defines
------------------------------------------------------------------------------*/
/* Input port ID of Gain module */
#define GAIN_MODULE_DATA_INPUT_PORT 0x2

/* Output port ID of Gain module */
#define GAIN_MODULE_DATA_OUTPUT_PORT 0x1

#define GAIN_MODULE_STACK_SIZE 4096
/*==============================================================================
   Module
==============================================================================*/

/** Module ID for Gain module  */
#define MODULE_ID_GAIN_MODULE 0x10015658
/** @h2xmlm_module       {"MODULE_ID_GAIN_MODULE",
                           MODULE_ID_GAIN_MODULE}
    @h2xmlm_displayName  {"Gain Control Module"}
    @h2xmlm_modMajorType {2}
    @h2xmlm_description  {Gain Module \n
                          - Supports following params:
                          - PARAM_ID_MODULE_ENABLE
                          - PARAM_ID_GAIN_MODULE_GAIN
                          - GAIN_PARAM_COEFF_ARR
                          - \n
                          - Supported Input Media Format:
                          - Data Format          : FIXED_POINT
                          - fmt_id               : Don't care
                          - Sample Rates         : Don't care
                          - Number of channels   : 1 to 32
                          - Channel type         : Don't care
                          - Bits per sample      : 16, 32
                          - Q format             : Don't care
                          - Interleaving         : de-interleaved unpacked
                          - Signed/unsigned      : Signed}
    @h2xmlm_dataMaxInputPorts        { 1 }
    @h2xmlm_dataInputPorts           { IN  = GAIN_MODULE_DATA_INPUT_PORT}
    @h2xmlm_dataMaxOutputPorts       { 1 }
    @h2xmlm_dataOutputPorts          { OUT = GAIN_MODULE_DATA_OUTPUT_PORT}
    @h2xmlm_supportedContTypes       {APM_CONTAINER_TYPE_SC, APM_CONTAINER_TYPE_GC}
    @h2xmlm_isOffloadable            {true}
    @h2xmlm_builtIn                  {false}
    @h2xmlm_stackSize                {GAIN_MODULE_STACK_SIZE }
    @h2xmlm_ToolPolicy              {Calibration}
    @h2xmlm_ctrlDynamicPortIntent {"Control port description" = INTENT_ID_GAIN_CONTROL, maxPorts = 1}
    @{                   <-- Start of the Module -->

    @h2xml_Select        {"param_id_module_enable_t"}
    @h2xmlm_InsertParameter

*/

/*==============================================================================
   API definitions
==============================================================================*/

#define PARAM_MOD_CONTROL_ENABLE 0x08001020

/*# @h2xmlp_parameter   {"PARAM_MOD_CONTROL_ENABLE", PARAM_MOD_CONTROL_ENABLE}
    @h2xmlp_description {ID for the Enable parameter used by any audio
                         processing module. This generic/common parameter is
                         used to configure or determine the state of any audio
                         processing module.}
    @h2xmlp_toolPolicy  {Calibration; RTC} */

#include "spf_begin_pack.h"
struct control_module_enable_t
{
   uint32_t enable;
   /**< Specifies whether the module is to be enabled or disabled.

        @valuesbul
        - 0 -- Disable (Default)
        - 1 -- Enable @tablebulletend */

   /*#< @h2xmle_description {Specifies whether the module is to be enabled or
                             disabled.}
        @h2xmle_rangeList   {"Disable"=0;
                             "Enable"=1}
        @h2xmle_default     {0}
        @h2xmle_policy      {Basic} */
}
#include "spf_end_pack.h"
;
typedef struct control_module_enable_t control_module_enable_t;


/* ID of the parameter used to set the gain */
#define PARAM_ID_GAIN_MODULE_GAIN 0x08001176

/** @h2xmlp_parameter   {"PARAM_ID_GAIN_MODULE_GAIN",
                         PARAM_ID_GAIN_MODULE_GAIN}
    @h2xmlp_description {Configures the gain}
    @h2xmlp_toolPolicy  {Calibration; RTC} */

#include "spf_begin_pack.h"
/* Payload for parameter param_id_gain_cfg_t */
typedef struct param_id_module_gain_cfg_t param_id_module_gain_cfg_t;

struct param_id_module_gain_cfg_t
{
   uint16_t gain;
   /**< @h2xmle_description {Linear gain (in Q13 format)}
        @h2xmle_dataFormat  {Q13}
        @h2xmle_default     {0x2000} */

   uint16_t reserved;
   /**< @h2xmle_description {Clients must set this field to 0.}
        @h2xmle_rangeList   {"0"=0} */
}
#include "spf_end_pack.h"
;

#define GAIN_PARAM_COEFF_ARR 0x0800122F

/* Structure definition for Parameter */
typedef struct control_tx_coeff_arr_t control_tx_coeff_arr_t;

/** @h2xmlp_parameter   {"GAIN_PARAM_COEFF_ARR",
                         GAIN_PARAM_COEFF_ARR}
    @h2xmlp_description {parameter used to send the coefficients to RX
                         from the control module.} */
#include "spf_begin_pack.h"

struct control_tx_coeff_arr_t
{
   unsigned char coeff_val[4096];
   /**< @h2xmle_description {Coefficient values}
        @h2xmle_default     {0x00}
        @h2xmle_range       {0..0xFF}
        
        @h2xmle_policy      {Basic} */
}
#include "spf_end_pack.h"
;

#define PARAM_ID_GAIN_MODULE_MUTE 0x08001230

/* Structure definition for Parameter */
typedef struct control_tx_mute_t control_tx_mute_t;

/** @h2xmlp_parameter   {"PARAM_ID_GAIN_MODULE_MUTE",
                         PARAM_ID_GAIN_MODULE_MUTE}
    @h2xmlp_description {parameter used to send the mute to RX
                         from the control module.} */
#include "spf_begin_pack.h"

struct control_tx_mute_t
{
   unsigned int mute;
   /*#< @h2xmle_description {Mute parameter}
        @h2xmle_rangeList   {"Disable"=0;
                             "Enable"=1}
        @h2xmle_default     {0}
        @h2xmle_policy      {Basic} */
}
#include "spf_end_pack.h"
;

/**  @}                   <-- End of the Module -->*/

#endif //_GAIN_MODULE_API_H_
