#ifndef _CAPI_FWK_EXTNS_SIGNAL_TRIGGERED_MODULE_H_
#define _CAPI_FWK_EXTNS_SIGNAL_TRIGGERED_MODULE_H_

/**
 *   \file capi_fwk_extns_signal_triggered_module.h
 *   \brief
 *        This file contains Signal Triggered Module's Extensions
 *
 * Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// clang-format off
/*
$Header: //source/qcom/qct/platform/adsp/proj/addons_audio/spf/incs/spf/interfaces/module/capi/capi_fwk_extns_signal_triggered_module.h#3 $
*/
// clang-format on

#include "capi_types.h"

/** @addtogroup capi_fw_ext_signal_trigger
@{ */

/*==============================================================================
  ?   Constants
==============================================================================*/

/** Unique identifier of the framework extension for the Signal Triggered Module (STM).

    This extension supports the following property and parameter IDs:
    - #FWK_EXTN_PROPERTY_ID_STM_TRIGGER
    - #FWK_EXTN_PROPERTY_ID_STM_CTRL
    - #FWK_EXTN_PARAM_ID_LATEST_TRIGGER_TIMESTAMP_PTR
*/
#define FWK_EXTN_STM 0x0A001003

/** ID of the custom property used to set a trigger.

    Through this STM extension, the framework sends a signal to the modules.
    This signal is triggered as follows:
    - Every interrupt for hardware endpoint modules
    - Every time the timer completes for timer-driven modules

    @msgpayload{capi_prop_stm_trigger_t}
    @table{weak__capi__prop__stm__trigger__t}
*/
#define FWK_EXTN_PROPERTY_ID_STM_TRIGGER 0x0A001004

/*==============================================================================
   Type definitions
==============================================================================*/

typedef struct capi_prop_stm_trigger_t capi_prop_stm_trigger_t;

/** @weakgroup weak_capi_prop_stm_trigger_t
@{ */
struct capi_prop_stm_trigger_t
{
   void *signal_ptr;
   /**< Pointer to the signal from the framework. */

   int32_t *raised_intr_counter_ptr;
   /**< The counter pointed by this pointer is incremented by the module 
    *   whenever the signal is set/raised by an interrupt.
    */
};
/** @} */ /* end_weakgroup weak_capi_prop_stm_trigger_t */

/*==============================================================================
   Constants
==============================================================================*/

/** ID of the custom property used to set the STM to a specific state.

    The framework uses this property ID to tell the module to start or stop.

    @msgpayload{capi_prop_stm_ctrl_t}
    @table{weak__capi__prop__stm__ctrl__t} @newpage
 */
#define FWK_EXTN_PROPERTY_ID_STM_CTRL 0x0A001005

/*==============================================================================
   Type definitions
==============================================================================*/

typedef struct capi_prop_stm_ctrl_t capi_prop_stm_ctrl_t;

/** @weakgroup weak_capi_prop_stm_ctrl_t
@{ */
struct capi_prop_stm_ctrl_t
{
   bool_t enable;
   /**< Specifies whether to enable the STM.

        @valuesbul
        - 0 -- FALSE (disable)
        - 1 -- TRUE (enable) @tablebulletend */
};
/** @} */ /* end_weakgroup weak_capi_prop_stm_ctrl_t */

/*==============================================================================
   Constants
==============================================================================*/
/** ID of the parameter used to get the pointer to the latest latched signal
    trigger or interrupt timestamp from the STM module.

    For hardware EP modules, this parameter corresponds to the pointer to the
    latest latched hardware interface's interrupt timestamp. If the trigger
    timestamp is not available, the module can return a NULL pointer or set the
    invalid flag.

    @msgpayload{capi_param_id_stm_latest_trigger_ts_ptr_t}
    @table{weak__capi__param__id__stm__latest__trigger__ts__ptr__t}

    @par Timestamp structure (stm_latest_trigger_ts_t)
    @table{weak__stm__latest__trigger__ts__t}
    */
#define FWK_EXTN_PARAM_ID_LATEST_TRIGGER_TIMESTAMP_PTR 0x0A001050

/*==============================================================================
   Type definitions
==============================================================================*/

typedef struct stm_latest_trigger_ts_t stm_latest_trigger_ts_t;

/** @weakgroup weak_stm_latest_trigger_ts_t
@{ */
struct stm_latest_trigger_ts_t
{
   bool_t   is_valid;
   /**< Specifies whether the timestamp is valid.

        @valuesbul
        - 0 -- Not valid
        - 1 -- Valid @tablebulletend */

   uint64_t timestamp;
   /**< Timestamp of the latest signal trigger or interrupt (updated in the
        #capi_vtbl_t::process() call). */
};
/** @} */ /* end_weakgroup weak_stm_latest_trigger_ts_t */

/* Structure for above property */
typedef struct capi_param_id_stm_latest_trigger_ts_ptr_t capi_param_id_stm_latest_trigger_ts_ptr_t;

/** @weakgroup weak_capi_param_id_stm_latest_trigger_ts_ptr_t
@{ */
struct capi_param_id_stm_latest_trigger_ts_ptr_t
{
   stm_latest_trigger_ts_t *ts_ptr;
   /**< Pointer to the timestamp structure. */
};
/** @} */ /* end_weakgroup weak_capi_param_id_stm_latest_trigger_ts_ptr_t */

/** @} */ /* end_addtogroup capi_fw_ext_signal_trigger */

#endif /* _CAPI_FWK_EXTNS_SIGNAL_TRIGGERED_MODULE_H_ */
