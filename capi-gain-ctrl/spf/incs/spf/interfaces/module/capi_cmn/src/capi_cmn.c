/**
 * \file capi_cmn.c
 * \brief
 *
 *
 * \copyright
 *  Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// clang-format off
/*
$Header: //source/qcom/qct/platform/adsp/proj/addons_audio/spf/incs/spf/interfaces/module/capi_cmn/src/capi_cmn.c#1 $
*/
// clang-format on

#include "capi_cmn.h"

/*=====================================================================
  Functions
 ======================================================================*/

capi_err_t capi_cmn_set_basic_properties(capi_proplist_t *           proplist_ptr,
                                         capi_heap_id_t *            heap_mem_ptr,
                                         capi_event_callback_info_t *cb_info_ptr,
                                         bool_t                      check_port_info)
{
   capi_err_t result = CAPI_EOK;
   uint32_t   i;
   if (NULL == proplist_ptr)
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: Set basic property received null property array");
      return CAPI_EBADPARAM;
   }

   capi_prop_t *prop_array = proplist_ptr->prop_ptr;
   for (i = 0; i < proplist_ptr->props_num; i++)
   {
      capi_buf_t *payload_ptr = &prop_array[i].payload;
      switch (prop_array[i].id)
      {
         case CAPI_HEAP_ID:
         {
            if (NULL == heap_mem_ptr)
            {
               break;
            }
            if (payload_ptr->actual_data_len >= sizeof(capi_heap_id_t))
            {
               capi_heap_id_t *data_ptr = (capi_heap_id_t *)payload_ptr->data_ptr;
               heap_mem_ptr->heap_id    = data_ptr->heap_id;
            }
            else
            {
               AR_MSG(DBG_ERROR_PRIO,
                      "capi_cmn: Set basic property id 0x%lx Bad param size %lu",
                      (uint32_t)prop_array[i].id,
                      payload_ptr->actual_data_len);
               CAPI_SET_ERROR(result, CAPI_ENEEDMORE);
            }
            break;
         }
         case CAPI_EVENT_CALLBACK_INFO:
         {
            if (NULL == cb_info_ptr)
            {
               break;
            }
            if (payload_ptr->actual_data_len >= sizeof(capi_event_callback_info_t))
            {
               capi_event_callback_info_t *data_ptr = (capi_event_callback_info_t *)payload_ptr->data_ptr;
               cb_info_ptr->event_cb                = data_ptr->event_cb;
               cb_info_ptr->event_context           = data_ptr->event_context;
            }
            else
            {
               AR_MSG(DBG_ERROR_PRIO,
                      "capi_cmn: Set basic property id 0x%lx Bad param size %lu",
                      (uint32_t)prop_array[i].id,
                      payload_ptr->actual_data_len);
               CAPI_SET_ERROR(result, CAPI_ENEEDMORE);
            }
            break;
         }
         case CAPI_PORT_NUM_INFO:
         {
            if (FALSE == check_port_info)
            {
               break;
            }

            if (payload_ptr->actual_data_len >= sizeof(capi_port_num_info_t))
            {
               capi_port_num_info_t *data_ptr = (capi_port_num_info_t *)payload_ptr->data_ptr;
               if ((CAPI_CMN_MAX_IN_PORTS < data_ptr->num_input_ports) ||
                   (CAPI_CMN_MAX_OUT_PORTS < data_ptr->num_output_ports))
               {
                  AR_MSG(DBG_ERROR_PRIO,
                         "capi_cmn:Set basic property id 0x%lx , number of input and output ports cannot be more "
                         "than %hu",
                         (uint32_t)prop_array[i].id,
                         CAPI_CMN_MAX_IN_PORTS);
                  CAPI_SET_ERROR(result, CAPI_EBADPARAM);
               }
            }
            else
            {
               AR_MSG(DBG_ERROR_PRIO,
                      "capi_cmn: Set basic property id 0x%lx Bad param size %lu",
                      (uint32_t)prop_array[i].id,
                      payload_ptr->actual_data_len);
               CAPI_SET_ERROR(result, CAPI_ENEEDMORE);
            }
            break;
         }
         default:
         {
            // Skipping this particular Set basic Property
            continue;
         }
      }
   }
   return result;
}

capi_err_t capi_cmn_get_basic_properties(capi_proplist_t *proplist_ptr, capi_basic_prop_t *mod_prop_ptr)
{
   capi_err_t result = CAPI_EOK;
   uint32_t   i;

   if (NULL == proplist_ptr)
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: Get basic property received null property array");
      return CAPI_EBADPARAM;
   }
   capi_prop_t *prop_array = proplist_ptr->prop_ptr;

   for (i = 0; i < proplist_ptr->props_num; i++)
   {
      capi_buf_t *payload_ptr = &prop_array[i].payload;
      if (NULL == payload_ptr->data_ptr)
      {
         AR_MSG(DBG_ERROR_PRIO, "capi_cmn: Get basic property id 0x%lx, received null buffer", prop_array[i].id);
         result |= CAPI_EBADPARAM;
         break;
      }
      switch (prop_array[i].id)
      {
         case CAPI_INIT_MEMORY_REQUIREMENT:
         {
            if (payload_ptr->max_data_len >= sizeof(capi_init_memory_requirement_t))
            {
               capi_init_memory_requirement_t *data_ptr = (capi_init_memory_requirement_t *)(payload_ptr->data_ptr);
               data_ptr->size_in_bytes                  = mod_prop_ptr->init_memory_req;
               payload_ptr->actual_data_len             = sizeof(capi_init_memory_requirement_t);
            }
            else
            {
               AR_MSG(DBG_ERROR_PRIO,
                      "capi_cmn: Get basic property id 0x%lx Bad param size %lu",
                      (uint32_t)prop_array[i].id,
                      payload_ptr->max_data_len);
               CAPI_SET_ERROR(result, CAPI_ENEEDMORE);
            }
            break;
         }
         case CAPI_STACK_SIZE:
         {
            if (payload_ptr->max_data_len >= sizeof(capi_stack_size_t))
            {
               capi_stack_size_t *data_ptr  = (capi_stack_size_t *)payload_ptr->data_ptr;
               data_ptr->size_in_bytes      = mod_prop_ptr->stack_size;
               payload_ptr->actual_data_len = sizeof(capi_stack_size_t);
            }
            else
            {
               AR_MSG(DBG_ERROR_PRIO,
                      "capi_cmn: Get basic property id 0x%lx Bad param size %lu",
                      (uint32_t)prop_array[i].id,
                      payload_ptr->max_data_len);
               CAPI_SET_ERROR(result, CAPI_ENEEDMORE);
            }
            break;
         }
         case CAPI_NUM_NEEDED_FRAMEWORK_EXTENSIONS:
         {
            if (payload_ptr->max_data_len >= sizeof(capi_num_needed_framework_extensions_t))
            {
               capi_num_needed_framework_extensions_t *data_ptr =
                  (capi_num_needed_framework_extensions_t *)payload_ptr->data_ptr;
               data_ptr->num_extensions     = mod_prop_ptr->num_fwk_extns;
               payload_ptr->actual_data_len = sizeof(capi_num_needed_framework_extensions_t);
            }
            else
            {
               AR_MSG(DBG_ERROR_PRIO,
                      "capi_cmn: Get basic property id 0x%lx Bad param size %lu",
                      (uint32_t)prop_array[i].id,
                      payload_ptr->max_data_len);
               CAPI_SET_ERROR(result, CAPI_ENEEDMORE);
            }
            break;
         }
         case CAPI_NEEDED_FRAMEWORK_EXTENSIONS:
         {
            uint32_t needed_size = sizeof(capi_framework_extension_id_t) * mod_prop_ptr->num_fwk_extns;
            if (payload_ptr->max_data_len >= needed_size)
            {
               capi_framework_extension_id_t *data_ptr = (capi_framework_extension_id_t *)payload_ptr->data_ptr;
               for (uint32_t num = 0; num < mod_prop_ptr->num_fwk_extns; num++)
               {
                  data_ptr[num].id = mod_prop_ptr->fwk_extn_ids_arr[num];
               }
               payload_ptr->actual_data_len = needed_size;
            }
            else
            {
               AR_MSG(DBG_ERROR_PRIO,
                      "capi_cmn: Get basic property id 0x%lx Bad param size %lu",
                      (uint32_t)prop_array[i].id,
                      payload_ptr->max_data_len);
               CAPI_SET_ERROR(result, CAPI_ENEEDMORE);
            }
            break;
         }
         case CAPI_IS_INPLACE:
         {
            if (payload_ptr->max_data_len >= sizeof(capi_is_inplace_t))
            {
               capi_is_inplace_t *data_ptr  = (capi_is_inplace_t *)payload_ptr->data_ptr;
               data_ptr->is_inplace         = mod_prop_ptr->is_inplace;
               payload_ptr->actual_data_len = sizeof(capi_is_inplace_t);
            }
            else
            {
               AR_MSG(DBG_ERROR_PRIO,
                      "capi_cmn: Get basic property id 0x%lx Bad param size %lu",
                      (uint32_t)prop_array[i].id,
                      payload_ptr->max_data_len);
               CAPI_SET_ERROR(result, CAPI_ENEEDMORE);
            }
            break;
         }
         case CAPI_REQUIRES_DATA_BUFFERING:
         {
            if (payload_ptr->max_data_len >= sizeof(capi_requires_data_buffering_t))
            {
               capi_requires_data_buffering_t *data_ptr = (capi_requires_data_buffering_t *)payload_ptr->data_ptr;
               data_ptr->requires_data_buffering        = mod_prop_ptr->req_data_buffering;
               payload_ptr->actual_data_len             = sizeof(capi_requires_data_buffering_t);
            }
            else
            {
               AR_MSG(DBG_ERROR_PRIO,
                      "capi_cmn: Get basic property id 0x%lx Bad param size %lu",
                      (uint32_t)prop_array[i].id,
                      payload_ptr->max_data_len);
               CAPI_SET_ERROR(result, CAPI_ENEEDMORE);
            }
            break;
         }
         case CAPI_MAX_METADATA_SIZE:
         {
            if (payload_ptr->max_data_len >= sizeof(capi_max_metadata_size_t))
            {
               capi_max_metadata_size_t *data_ptr = (capi_max_metadata_size_t *)payload_ptr->data_ptr;
               data_ptr->size_in_bytes            = mod_prop_ptr->max_metadata_size;
               payload_ptr->actual_data_len       = sizeof(capi_max_metadata_size_t);
            }
            else
            {
               AR_MSG(DBG_ERROR_PRIO,
                      "capi_cmn: Get basic property id 0x%lx Bad param size %lu",
                      (uint32_t)prop_array[i].id,
                      payload_ptr->max_data_len);
               CAPI_SET_ERROR(result, CAPI_ENEEDMORE);
            }
            break;
         }
         default:
         {
            continue;
         }
      }
   }
   return result;
}

capi_err_t capi_cmn_update_algo_delay_event(capi_event_callback_info_t *cb_info_ptr, uint32_t delay_in_us)
{
   if (NULL == cb_info_ptr->event_cb)
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn : capi event callback is not set, unable to raise delay event!");
      return CAPI_EBADPARAM;
   }
   capi_err_t                     result = CAPI_EOK;
   capi_event_algorithmic_delay_t event;
   event.delay_in_us = delay_in_us;
   capi_event_info_t event_info;
   event_info.port_info.is_valid      = FALSE;
   event_info.payload.actual_data_len = event_info.payload.max_data_len = sizeof(event);
   event_info.payload.data_ptr                                          = (int8_t *)(&event);
   result = cb_info_ptr->event_cb(cb_info_ptr->event_context, CAPI_EVENT_ALGORITHMIC_DELAY, &event_info);
   if (CAPI_FAILED(result))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Failed to send delay update event with %lu", result);
   }
   return result;
}

capi_err_t capi_cmn_update_hw_acc_proc_delay_event(capi_event_callback_info_t *cb_info_ptr, uint32_t delay_in_us)
{
   if (NULL == cb_info_ptr->event_cb)
   {
      AR_MSG(DBG_ERROR_PRIO,
             "capi_cmn : capi event callback is not set, unable to raise HW Accelerator proc delay event!");
      return CAPI_EBADPARAM;
   }
   capi_err_t                      result = CAPI_EOK;
   capi_event_hw_accl_proc_delay_t event;
   event.delay_in_us = delay_in_us;
   capi_event_info_t event_info;
   event_info.port_info.is_valid      = FALSE;
   event_info.payload.actual_data_len = event_info.payload.max_data_len = sizeof(event);
   event_info.payload.data_ptr                                          = (int8_t *)(&event);
   result = cb_info_ptr->event_cb(cb_info_ptr->event_context, CAPI_EVENT_HW_ACCL_PROC_DELAY, &event_info);
   if (CAPI_FAILED(result))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Failed to send HW Accelerator proc delay update event with %lu", result);
   }
   return result;
}

capi_err_t capi_cmn_raise_dynamic_inplace_event(capi_event_callback_info_t *cb_info_ptr, bool_t is_inplace)
{
   if (NULL == cb_info_ptr->event_cb)
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Event callback is not set, Unable to raise kpps event!");
      return CAPI_EBADPARAM;
   }

   capi_err_t                          result = CAPI_EOK;
   capi_event_dynamic_inplace_change_t event;
   event.is_inplace = is_inplace;
   capi_event_info_t event_info;
   event_info.port_info.is_valid      = FALSE;
   event_info.payload.actual_data_len = event_info.payload.max_data_len = sizeof(capi_event_dynamic_inplace_change_t);
   event_info.payload.data_ptr                                          = (int8_t *)(&event);
   result = cb_info_ptr->event_cb(cb_info_ptr->event_context, CAPI_EVENT_DYNAMIC_INPLACE_CHANGE, &event_info);
   if (CAPI_FAILED(result))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Failed to send dynamic inplace %lu update event", is_inplace);
   }
   return result;
}

capi_err_t capi_cmn_update_kpps_event(capi_event_callback_info_t *cb_info_ptr, uint32_t kpps)
{
   if (NULL == cb_info_ptr->event_cb)
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Event callback is not set, Unable to raise kpps event!");
      return CAPI_EBADPARAM;
   }

   capi_err_t        result = CAPI_EOK;
   capi_event_KPPS_t event;
   event.KPPS = kpps;
   capi_event_info_t event_info;
   event_info.port_info.is_valid      = FALSE;
   event_info.payload.actual_data_len = event_info.payload.max_data_len = sizeof(capi_event_KPPS_t);
   event_info.payload.data_ptr                                          = (int8_t *)(&event);
   result = cb_info_ptr->event_cb(cb_info_ptr->event_context, CAPI_EVENT_KPPS, &event_info);
   if (CAPI_FAILED(result))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Failed to send KPPS update event with %lu", result);
   }
   return result;
}

capi_err_t capi_cmn_update_bandwidth_event(capi_event_callback_info_t *cb_info_ptr,
                                           uint32_t                    code_bandwidth,
                                           uint32_t                    data_bandwidth)
{
   if (NULL == cb_info_ptr->event_cb)
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Event callback is not set,Unable to raise bandwidth event!");
      return CAPI_EBADPARAM;
   }

   capi_err_t result = CAPI_EOK;

   capi_event_bandwidth_t event;
   event.code_bandwidth = code_bandwidth;
   event.data_bandwidth = data_bandwidth;
   capi_event_info_t event_info;
   event_info.port_info.is_valid      = FALSE;
   event_info.payload.actual_data_len = sizeof(event);
   event_info.payload.max_data_len    = sizeof(event);
   event_info.payload.data_ptr        = (int8_t *)(&event);
   result = cb_info_ptr->event_cb(cb_info_ptr->event_context, CAPI_EVENT_BANDWIDTH, &event_info);
   if (CAPI_FAILED(result))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Failed to send bandwidth update event with %lu", result);
   }
   return result;
}

capi_err_t capi_cmn_update_process_check_event(capi_event_callback_info_t *cb_info_ptr, uint32_t process_check)
{
   capi_err_t result = CAPI_EOK;
   if (NULL == cb_info_ptr->event_cb)
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Event callback is not set, Unable to raise process check event!");
      return CAPI_EBADPARAM;
   }

   capi_event_process_state_t event;
   event.is_enabled = process_check;

   capi_event_info_t event_info;
   event_info.port_info.is_valid      = FALSE;
   event_info.payload.actual_data_len = event_info.payload.max_data_len = sizeof(event);
   event_info.payload.data_ptr                                          = (int8_t *)(&event);
   result = cb_info_ptr->event_cb(cb_info_ptr->event_context, CAPI_EVENT_PROCESS_STATE, &event_info);
   if (CAPI_FAILED(result))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Failed to send process update event with %d", result);
   }
   else
   {
      AR_MSG(DBG_HIGH_PRIO, "capi_cmn : Raising process update event with enable set to %d", process_check);
   }
   return result;
}

capi_err_t capi_cmn_update_port_data_threshold_event(capi_event_callback_info_t *cb_info_ptr,
                                                     uint32_t                    threshold_bytes,
                                                     bool_t                      is_input_port,
                                                     uint32_t                    port_index)
{
   capi_err_t result = CAPI_EOK;

   if (NULL == cb_info_ptr->event_cb)
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Event callback is not set, Unable to raise threshold event!");
      return CAPI_EBADPARAM;
   }

   capi_port_data_threshold_change_t evnt;
   evnt.new_threshold_in_bytes = threshold_bytes;

   capi_event_info_t event_info;
   event_info.port_info.is_input_port = is_input_port;
   event_info.port_info.is_valid      = TRUE;
   event_info.port_info.port_index    = port_index;
   event_info.payload.actual_data_len = sizeof(capi_port_data_threshold_change_t);
   event_info.payload.data_ptr        = (int8_t *)&evnt;
   event_info.payload.max_data_len    = sizeof(capi_port_data_threshold_change_t);

   result = cb_info_ptr->event_cb(cb_info_ptr->event_context, CAPI_EVENT_PORT_DATA_THRESHOLD_CHANGE, &event_info);

   if (CAPI_FAILED(result))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: Failed to send output media format updated event V2 with %d", result);
   }
   return result;
}

capi_err_t capi_cmn_output_media_fmt_event_v1(capi_event_callback_info_t *cb_info_ptr,
                                              capi_media_fmt_v1_t *       out_media_fmt,
                                              bool_t                      is_input_port,
                                              uint32_t                    port_index)
{
   capi_err_t result = CAPI_EOK;
   if ((NULL == cb_info_ptr->event_cb) || (NULL == out_media_fmt))
   {
      AR_MSG(DBG_ERROR_PRIO,
             "capi_cmn : Event callback is not set or media fmt is NULL, Unable to raise output "
             "media fmt event!");
      return CAPI_EBADPARAM;
   }

   capi_event_info_t event_info;
   event_info.port_info.port_index    = port_index;
   event_info.port_info.is_valid      = TRUE;
   event_info.port_info.is_input_port = is_input_port;

   event_info.payload.actual_data_len = sizeof(capi_media_fmt_v1_t);
   event_info.payload.max_data_len    = sizeof(capi_media_fmt_v1_t);
   event_info.payload.data_ptr        = (int8_t *)(out_media_fmt);
   result = cb_info_ptr->event_cb(cb_info_ptr->event_context, CAPI_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED, &event_info);

   if (CAPI_FAILED(result))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: Failed to send output media format updated event with %d", result);
   }
   return result;
}

capi_err_t capi_cmn_output_media_fmt_event_v2(capi_event_callback_info_t *cb_info_ptr,
                                              capi_media_fmt_v2_t *       out_media_fmt,
                                              bool_t                      is_input_port,
                                              uint32_t                    port_index)
{

   capi_err_t result = CAPI_EOK;
   if ((NULL == cb_info_ptr->event_cb) || (NULL == out_media_fmt))
   {
      AR_MSG(DBG_ERROR_PRIO,
             "capi_cmn : Event callback is not set or media fmt is NULL, Unable to raise output "
             "media fmt v2 event!");
      return CAPI_EBADPARAM;
   }

   capi_event_info_t event_info;
   event_info.port_info.port_index    = port_index;
   event_info.port_info.is_valid      = TRUE;
   event_info.port_info.is_input_port = is_input_port;

   uint32_t total_size =
      CAPI_MF_V2_MIN_SIZE +
      (CAPI_ALIGN_4_BYTE(sizeof(out_media_fmt->channel_type[0]) * out_media_fmt->format.num_channels));
   event_info.payload.actual_data_len = total_size;
   event_info.payload.max_data_len    = sizeof(capi_media_fmt_v2_t);
   event_info.payload.data_ptr        = (int8_t *)(out_media_fmt);
   result = cb_info_ptr->event_cb(cb_info_ptr->event_context, CAPI_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED_V2, &event_info);

   if (CAPI_FAILED(result))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: Failed to send output media format updated event V2 with %d", result);
   }
   return result;
}

capi_err_t capi_cmn_raw_output_media_fmt_event(capi_event_callback_info_t *cb_info_ptr,
                                               capi_cmn_raw_media_fmt_t *  out_media_fmt,
                                               bool_t                      is_input_port,
                                               uint32_t                    port_index)
{
   capi_err_t result = CAPI_EOK;
   if ((NULL == cb_info_ptr->event_cb) || (NULL == out_media_fmt))
   {
      AR_MSG(DBG_ERROR_PRIO,
             "capi_cmn : Event callback is not set or media fmt is NULL, Unable to raise raw output "
             "media fmt event!");
      return CAPI_EBADPARAM;
   }

   capi_event_info_t event_info;
   event_info.port_info.port_index    = port_index;
   event_info.port_info.is_valid      = TRUE;
   event_info.port_info.is_input_port = is_input_port;

   event_info.payload.actual_data_len = sizeof(capi_set_get_media_format_t) + sizeof(capi_raw_compressed_data_format_t);
   event_info.payload.max_data_len    = event_info.payload.actual_data_len;
   event_info.payload.data_ptr        = (int8_t *)(out_media_fmt);
   result = cb_info_ptr->event_cb(cb_info_ptr->event_context, CAPI_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED_V2, &event_info);

   if (CAPI_FAILED(result))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: Failed to send raw output media format updated event with %d", result);
   }
   else
   {
      AR_MSG(DBG_HIGH_PRIO,
             "capi_cmn: Raising raw out mf with data format %d, bitstream format %x ",
             out_media_fmt->header.format_header.data_format,
             out_media_fmt->format.bitstream_format);
   }
   return result;
}

capi_err_t capi_cmn_init_media_fmt_v1(capi_media_fmt_v1_t *media_fmt_ptr)
{
   media_fmt_ptr->header.format_header.data_format = CAPI_FIXED_POINT;
   media_fmt_ptr->format.bits_per_sample           = CAPI_DATA_FORMAT_INVALID_VAL;
   media_fmt_ptr->format.bitstream_format          = CAPI_DATA_FORMAT_INVALID_VAL;
   media_fmt_ptr->format.data_interleaving         = CAPI_INVALID_INTERLEAVING;
   media_fmt_ptr->format.data_is_signed            = CAPI_DATA_FORMAT_INVALID_VAL;
   media_fmt_ptr->format.num_channels              = CAPI_DATA_FORMAT_INVALID_VAL;
   media_fmt_ptr->format.q_factor                  = CAPI_DATA_FORMAT_INVALID_VAL;
   media_fmt_ptr->format.sampling_rate             = CAPI_DATA_FORMAT_INVALID_VAL;

   for (uint32_t j = 0; j < CAPI_MAX_CHANNELS; j++)
   {
      media_fmt_ptr->format.channel_type[j] = (uint16_t)CAPI_DATA_FORMAT_INVALID_VAL;
   }
   return CAPI_EOK;
}

capi_err_t capi_cmn_init_media_fmt_v2(capi_media_fmt_v2_t *media_fmt_ptr)
{
   media_fmt_ptr->header.format_header.data_format = CAPI_FIXED_POINT;
   media_fmt_ptr->format.minor_version             = CAPI_DATA_FORMAT_INVALID_VAL;
   media_fmt_ptr->format.bits_per_sample           = CAPI_DATA_FORMAT_INVALID_VAL;
   media_fmt_ptr->format.bitstream_format          = CAPI_DATA_FORMAT_INVALID_VAL;
   media_fmt_ptr->format.data_interleaving         = CAPI_INVALID_INTERLEAVING;
   media_fmt_ptr->format.data_is_signed            = CAPI_DATA_FORMAT_INVALID_VAL;
   media_fmt_ptr->format.num_channels              = CAPI_DATA_FORMAT_INVALID_VAL;
   media_fmt_ptr->format.q_factor                  = CAPI_DATA_FORMAT_INVALID_VAL;
   media_fmt_ptr->format.sampling_rate             = CAPI_DATA_FORMAT_INVALID_VAL;

   for (uint32_t j = 0; j < CAPI_MAX_CHANNELS_V2; j++)
   {
      media_fmt_ptr->channel_type[j] = (uint16_t)CAPI_DATA_FORMAT_INVALID_VAL;
   }
   return CAPI_EOK;
}

capi_err_t capi_cmn_handle_get_output_media_fmt_v1(capi_prop_t *prop_ptr, capi_media_fmt_v1_t *media_fmt_ptr)
{
   capi_err_t result = CAPI_EOK;
   if (prop_ptr->payload.max_data_len >= sizeof(capi_media_fmt_v1_t))
   {
      capi_media_fmt_v1_t *data_ptr = (capi_media_fmt_v1_t *)prop_ptr->payload.data_ptr;
      if ((FALSE == prop_ptr->port_info.is_valid) && (TRUE == prop_ptr->port_info.is_input_port))
      {
         AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Get output media fmt port id not valid or input port");
         result = CAPI_EBADPARAM;
      }
      memscpy(data_ptr, sizeof(capi_media_fmt_v1_t), media_fmt_ptr, sizeof(capi_media_fmt_v1_t));
      prop_ptr->payload.actual_data_len = sizeof(capi_media_fmt_v1_t);
   }
   else
   {
      AR_MSG(DBG_ERROR_PRIO,
             "capi_cmn: Get property_id 0x%lx, Bad param size %lu",
             (uint32_t)prop_ptr->id,
             prop_ptr->payload.max_data_len);
      prop_ptr->payload.actual_data_len = 0;
      return CAPI_ENEEDMORE;
   }
   return result;
}

capi_err_t capi_cmn_handle_get_output_media_fmt_v2(capi_prop_t *prop_ptr, capi_media_fmt_v2_t *media_fmt_ptr)
{
   capi_err_t result = CAPI_EOK;
   uint32_t   total_size =
      CAPI_MF_V2_MIN_SIZE +
      (CAPI_ALIGN_4_BYTE(sizeof(media_fmt_ptr->channel_type[0]) * media_fmt_ptr->format.num_channels));

   if (prop_ptr->payload.max_data_len >= total_size)
   {
      capi_media_fmt_v2_t *data_ptr = (capi_media_fmt_v2_t *)prop_ptr->payload.data_ptr;
      if ((FALSE == prop_ptr->port_info.is_valid) && (TRUE == prop_ptr->port_info.is_input_port))
      {
         AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Get output media fmt v2 port id not valid or input port");
         result = CAPI_EBADPARAM;
      }
      memscpy(data_ptr, total_size, media_fmt_ptr, total_size);
      prop_ptr->payload.actual_data_len = total_size;
   }
   else
   {
      AR_MSG(DBG_ERROR_PRIO,
             "capi_cmn: Get property_id 0x%lx, Bad param size %lu",
             (uint32_t)prop_ptr->id,
             prop_ptr->payload.max_data_len);
      prop_ptr->payload.actual_data_len = 0;
      return CAPI_ENEEDMORE;
   }
   return result;
}

capi_err_t capi_cmn_handle_get_port_threshold(capi_prop_t *prop_ptr, uint32_t threshold)
{
   capi_err_t result = CAPI_EOK;
   if (prop_ptr->payload.max_data_len >= sizeof(capi_port_data_threshold_t))
   {
      capi_port_data_threshold_t *data_ptr = (capi_port_data_threshold_t *)prop_ptr->payload.data_ptr;
      if (!prop_ptr->port_info.is_valid)
      {
         AR_MSG(DBG_ERROR_PRIO, "capi_cmn : Get port threshold port id not valid");
         result = CAPI_EBADPARAM;
      }
      if (0 != prop_ptr->port_info.port_index)
      {
         AR_MSG(DBG_ERROR_PRIO,
                "capi_cmn: Get property_id 0x%lx, max in/out port is 1. asking for %lu",
                (uint32_t)prop_ptr->id,
                prop_ptr->port_info.port_index);
         result = CAPI_EBADPARAM;
      }
      data_ptr->threshold_in_bytes      = threshold;
      prop_ptr->payload.actual_data_len = sizeof(capi_port_data_threshold_t);
   }
   else
   {
      AR_MSG(DBG_ERROR_PRIO,
             "capi_cmn: Get property_id 0x%lx, Bad param size %lu",
             (uint32_t)prop_ptr->id,
             prop_ptr->payload.max_data_len);
      prop_ptr->payload.actual_data_len = 0;
      return CAPI_ENEEDMORE;
   }
   return result;
}

capi_err_t capi_cmn_validate_client_pcm_media_format(const payload_media_fmt_pcm_t *pcm_fmt_ptr)
{
   capi_err_t result = CAPI_EOK;

   bool_t bps_related_err = FALSE;
   if (((pcm_fmt_ptr->num_channels == 0) || (pcm_fmt_ptr->num_channels > CAPI_MAX_CHANNELS_V2)))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: unsupported number of channels %lu", pcm_fmt_ptr->num_channels);
      return CAPI_EBADPARAM;
   }

   if ((0 == pcm_fmt_ptr->sample_rate) || (SAMPLE_RATE_384K < pcm_fmt_ptr->sample_rate))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: unsupported sampling rate %lu", pcm_fmt_ptr->sample_rate);
      return CAPI_EBADPARAM;
   }

   switch (pcm_fmt_ptr->bit_width)
   {
      case BIT_WIDTH_16:
      {
         if (!(BITS_PER_SAMPLE_16 == pcm_fmt_ptr->bits_per_sample) && (PCM_Q_FACTOR_15 == pcm_fmt_ptr->q_factor))
         {
            bps_related_err = TRUE;
         }
         break;
      }
      case BIT_WIDTH_24:
      {
         if (BITS_PER_SAMPLE_24 == pcm_fmt_ptr->bits_per_sample)
         {
            if (PCM_Q_FACTOR_23 != pcm_fmt_ptr->q_factor)
            {
               bps_related_err = TRUE;
            }
         }
         else if (BITS_PER_SAMPLE_32 == pcm_fmt_ptr->bits_per_sample)
         {
            if (!(((PCM_Q_FACTOR_27 == pcm_fmt_ptr->q_factor)) ||
                  ((PCM_Q_FACTOR_23 == pcm_fmt_ptr->q_factor) && (PCM_LSB_ALIGNED == pcm_fmt_ptr->alignment)) ||
                  ((PCM_Q_FACTOR_31 == pcm_fmt_ptr->q_factor) && (PCM_MSB_ALIGNED == pcm_fmt_ptr->alignment))))
            {
               bps_related_err = TRUE;
            }
         }
         else
         {
            bps_related_err = TRUE;
         }
         break;
      }
      case BIT_WIDTH_32:
      {
         if (!((BITS_PER_SAMPLE_32 == pcm_fmt_ptr->bits_per_sample)) && ((PCM_Q_FACTOR_31 == pcm_fmt_ptr->q_factor)))
         {
            bps_related_err = TRUE;
         }
         break;
      }
      default:
      {
         bps_related_err = TRUE;
         break;
      }
   }

   if (bps_related_err)
   {
      AR_MSG(DBG_ERROR_PRIO,
             "capi_cmn: unsupported bits per sample %lu, sample word size %lu, or q format %lu ",
             pcm_fmt_ptr->bit_width,
             pcm_fmt_ptr->bits_per_sample,
             pcm_fmt_ptr->q_factor);

      return CAPI_EBADPARAM;
   }

   if ((PCM_LITTLE_ENDIAN != pcm_fmt_ptr->endianness) && (PCM_BIG_ENDIAN != pcm_fmt_ptr->endianness))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: unsupported endianness %lu", pcm_fmt_ptr->endianness);
      return CAPI_EBADPARAM;
   }

   return result;
}

capi_err_t capi_cmn_validate_client_pcm_output_cfg(const payload_pcm_output_format_cfg_t *pcm_cfg_ptr)
{
   capi_err_t result = CAPI_EOK;

   bool_t bps_related_err     = FALSE;
   bool_t bps_native_or_unset = FALSE;

   if ((PARAM_VAL_NATIVE != pcm_cfg_ptr->endianness) && (PARAM_VAL_UNSET != pcm_cfg_ptr->endianness) &&
       (PCM_LITTLE_ENDIAN != pcm_cfg_ptr->endianness) && (PCM_BIG_ENDIAN != pcm_cfg_ptr->endianness))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: unsupported endianness %ld", pcm_cfg_ptr->endianness);
      return CAPI_EBADPARAM;
   }

   if ((PARAM_VAL_NATIVE != pcm_cfg_ptr->interleaved) && (PARAM_VAL_UNSET != pcm_cfg_ptr->interleaved) &&
       (PCM_INTERLEAVED != pcm_cfg_ptr->interleaved) && (PCM_DEINTERLEAVED_PACKED != pcm_cfg_ptr->interleaved) &&
       (PCM_DEINTERLEAVED_UNPACKED != pcm_cfg_ptr->interleaved))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: unsupported interleaving %ld.", pcm_cfg_ptr->interleaved);
      return CAPI_EBADPARAM;
   }

   // Not native, not unset and also not in valid range
   if ((PARAM_VAL_NATIVE != pcm_cfg_ptr->num_channels) && (PARAM_VAL_UNSET != pcm_cfg_ptr->num_channels) &&
       (!(CAPI_MAX_CHANNELS_V2 >= pcm_cfg_ptr->num_channels) && (PARAM_VAL_INVALID < pcm_cfg_ptr->num_channels)))
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: unsupported number of channels %ld", pcm_cfg_ptr->num_channels);
      return CAPI_EBADPARAM;
   }

   if ((PARAM_VAL_NATIVE != pcm_cfg_ptr->bit_width) && (PARAM_VAL_UNSET != pcm_cfg_ptr->bit_width) &&
       (BIT_WIDTH_16 != pcm_cfg_ptr->bit_width) && (BIT_WIDTH_24 != pcm_cfg_ptr->bit_width) &&
       (BIT_WIDTH_32 != pcm_cfg_ptr->bit_width))
   {
      bps_related_err = TRUE;
   }

   if ((PARAM_VAL_NATIVE != pcm_cfg_ptr->bits_per_sample) && (PARAM_VAL_UNSET != pcm_cfg_ptr->bits_per_sample) &&
       (16 != pcm_cfg_ptr->bits_per_sample) && (24 != pcm_cfg_ptr->bits_per_sample) &&
       (32 != pcm_cfg_ptr->bits_per_sample))
   {
      bps_related_err = TRUE;
   }

   if ((PARAM_VAL_NATIVE != pcm_cfg_ptr->alignment) && (PARAM_VAL_UNSET != pcm_cfg_ptr->alignment) &&
       (PCM_LSB_ALIGNED != pcm_cfg_ptr->alignment) && (PCM_MSB_ALIGNED != pcm_cfg_ptr->alignment))
   {
      bps_related_err = TRUE;
   }

   if ((PARAM_VAL_NATIVE != pcm_cfg_ptr->q_factor) && (PARAM_VAL_UNSET != pcm_cfg_ptr->q_factor) &&
       (PCM_Q_FACTOR_15 != pcm_cfg_ptr->q_factor) && (PCM_Q_FACTOR_23 != pcm_cfg_ptr->q_factor) &&
       (PCM_Q_FACTOR_27 != pcm_cfg_ptr->q_factor) && (PCM_Q_FACTOR_31 != pcm_cfg_ptr->q_factor))
   {
      bps_related_err = TRUE;
   }

   // Ensure if one is native mode, all others are native mode. If one is unset all others are unset
   if (((PARAM_VAL_NATIVE == pcm_cfg_ptr->bit_width) || (PARAM_VAL_UNSET == pcm_cfg_ptr->bit_width)) ||
       ((PARAM_VAL_NATIVE == pcm_cfg_ptr->bits_per_sample) || (PARAM_VAL_UNSET == pcm_cfg_ptr->bits_per_sample)) ||
       ((PARAM_VAL_NATIVE == pcm_cfg_ptr->alignment) || (PARAM_VAL_UNSET == pcm_cfg_ptr->alignment)) ||
       ((PARAM_VAL_NATIVE == pcm_cfg_ptr->q_factor) || (PARAM_VAL_UNSET == pcm_cfg_ptr->q_factor)))
   {
      bps_native_or_unset = TRUE;
      if ((pcm_cfg_ptr->bits_per_sample != pcm_cfg_ptr->bit_width) ||
          (pcm_cfg_ptr->alignment != pcm_cfg_ptr->bit_width) || (pcm_cfg_ptr->q_factor != pcm_cfg_ptr->bit_width))
      {
         bps_related_err = TRUE;
      }
   }

   if (!bps_related_err && !bps_native_or_unset)
   {
      switch (pcm_cfg_ptr->bit_width)
      {
         case 16:
         {
            if (!(16 == pcm_cfg_ptr->bits_per_sample) && (15 == pcm_cfg_ptr->q_factor))
            {
               bps_related_err = TRUE;
            }
            break;
         }
         case 24:
         {
            if (24 == pcm_cfg_ptr->bits_per_sample)
            {
               if (23 != pcm_cfg_ptr->q_factor)
               {
                  bps_related_err = TRUE;
               }
            }
            else if (32 == pcm_cfg_ptr->bits_per_sample)
            {
               if (!(((27 == pcm_cfg_ptr->q_factor)) ||
                     ((23 == pcm_cfg_ptr->q_factor) && (PCM_LSB_ALIGNED == pcm_cfg_ptr->alignment)) ||
                     ((31 == pcm_cfg_ptr->q_factor) && (PCM_MSB_ALIGNED == pcm_cfg_ptr->alignment))))
               {
                  bps_related_err = TRUE;
               }
            }
            else
            {
               bps_related_err = TRUE;
            }
            break;
         }
         case 32:
         {
            if (!((32 == pcm_cfg_ptr->bits_per_sample)) && ((31 == pcm_cfg_ptr->q_factor)))
            {
               bps_related_err = TRUE;
            }
            break;
         }
         default:
         {
            bps_related_err = TRUE;
            break;
         }
      }
   }

   if (bps_related_err)
   {
      AR_MSG(DBG_ERROR_PRIO,
             "capi_cmn: unsupported bit_width %ld, bits_per_sample %ld, or q_factor %ld ",
             pcm_cfg_ptr->bit_width,
             pcm_cfg_ptr->bits_per_sample,
             pcm_cfg_ptr->q_factor);

      return CAPI_EBADPARAM;
   }

   return result;
}

/**
 * Returns true if the media formats are equal.
 */
bool_t capi_cmn_media_fmt_equal(capi_media_fmt_v2_t *media_fmt_1_ptr, capi_media_fmt_v2_t *media_fmt_2_ptr)
{
   bool_t are_dif = FALSE;

   // clang-format off
   are_dif |= (media_fmt_1_ptr->header.format_header.data_format != media_fmt_2_ptr->header.format_header.data_format);
   are_dif |= (media_fmt_1_ptr->format.bits_per_sample           != media_fmt_2_ptr->format.bits_per_sample);
   are_dif |= (media_fmt_1_ptr->format.bitstream_format          != media_fmt_2_ptr->format.bitstream_format);
   are_dif |= (media_fmt_1_ptr->format.data_interleaving         != media_fmt_2_ptr->format.data_interleaving);
   are_dif |= (media_fmt_1_ptr->format.data_is_signed            != media_fmt_2_ptr->format.data_is_signed);
   are_dif |= (media_fmt_1_ptr->format.num_channels              != media_fmt_2_ptr->format.num_channels);
   are_dif |= (media_fmt_1_ptr->format.q_factor                  != media_fmt_2_ptr->format.q_factor);
   are_dif |= (media_fmt_1_ptr->format.sampling_rate             != media_fmt_2_ptr->format.sampling_rate);
   // clang-format on

   return !are_dif;
}

/*------------------------------------------------------------------------
  Function name: capi_cmn_data_fmt_map
DESCRIPTION: Function to get the data fmt map
 * -----------------------------------------------------------------------*/
capi_err_t capi_cmn_data_fmt_map(uint32_t *in_format, capi_media_fmt_v2_t *media_fmt)
{
   capi_err_t capi_result = CAPI_EOK;

   if (NULL == media_fmt)
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: Null pointer received in data format map");
      return CAPI_EBADPARAM;
   }

   switch (*in_format)
   {
      case DATA_FORMAT_FIXED_POINT:
      {
         media_fmt->header.format_header.data_format = CAPI_FIXED_POINT;
         break;
      }
      case DATA_FORMAT_IEC61937_PACKETIZED:
      {
         media_fmt->header.format_header.data_format = CAPI_IEC61937_PACKETIZED;
         break;
      }
      case DATA_FORMAT_IEC60958_PACKETIZED:
      {
         media_fmt->header.format_header.data_format = CAPI_IEC60958_PACKETIZED;
         break;
      }
      case DATA_FORMAT_IEC60958_PACKETIZED_NON_LINEAR:
      {
         media_fmt->header.format_header.data_format = CAPI_IEC60958_PACKETIZED_NON_LINEAR;
         break;
      }
      case DATA_FORMAT_DSD_OVER_PCM:
      {
         media_fmt->header.format_header.data_format = CAPI_DSD_DOP_PACKETIZED;
         break;
      }
      case DATA_FORMAT_GENERIC_COMPRESSED:
      {
         media_fmt->header.format_header.data_format = CAPI_GENERIC_COMPRESSED;
         break;
      }
      case DATA_FORMAT_RAW_COMPRESSED:
      {
         media_fmt->header.format_header.data_format = CAPI_RAW_COMPRESSED;
         break;
      }
      case DATA_FORMAT_COMPR_OVER_PCM_PACKETIZED:
      {
         media_fmt->header.format_header.data_format = CAPI_COMPR_OVER_PCM_PACKETIZED;
         break;
      }
      default:
      {
         AR_MSG(DBG_ERROR_PRIO, "capi_cmn: Unsupported data format %lu", *in_format);
         capi_result = CAPI_EUNSUPPORTED;
         break;
      }
   }

   return capi_result;
}

/*------------------------------------------------------------------------
  Function name: capi_cmn_remove_dec_zeroes
  DESCRIPTION: Function to remove zeroes from deoder output's beginning
  and end to support Gapless.
  bytes_to_remove_per_channel_ptr -  pointer to number of bytes to be removed per channel. Also it is used to return the
 amount of bytes left (and to be removed in next iteration
 * -----------------------------------------------------------------------*/
capi_err_t capi_cmn_gapless_remove_zeroes(uint32_t *            bytes_to_remove_per_channel_ptr,
                                          capi_media_fmt_v2_t * out_mf_ptr,
                                          capi_stream_data_t *  output[],
                                          bool_t                initial,
                                          module_cmn_md_list_t *metadata_list_ptr)
{
   if (metadata_list_ptr)
   {
      AR_MSG(DBG_ERROR_PRIO, "capi_cmn: metadata propagation currently not handled here ");
   }

   if ((bytes_to_remove_per_channel_ptr == NULL) || (out_mf_ptr == NULL))
   {
      return CAPI_EBADPARAM;
   }

   uint32_t valid_bytes_to_remove_per_channel = *bytes_to_remove_per_channel_ptr;
   int8_t * addr_read_ptr = NULL, *addr_write_ptr = NULL;
   uint32_t valid_bytes_in_buffer = 0;

   // it corresponds to the actual number of bytes present in buffer after decoding
   valid_bytes_in_buffer = output[0]->buf_ptr->actual_data_len;

   uint32_t valid_bytes_per_channel = 0;
   if ((out_mf_ptr->format.data_interleaving == CAPI_DEINTERLEAVED_PACKED) ||
       (out_mf_ptr->format.data_interleaving == CAPI_INTERLEAVED))
   {
      valid_bytes_per_channel = (valid_bytes_in_buffer) / out_mf_ptr->format.num_channels;

      if (valid_bytes_to_remove_per_channel > valid_bytes_per_channel)
      {
         // We drop the entire decoded frame, because the amount of data to be
         // removed is already greater than the data present in one decoded frame

         AR_MSG(DBG_MED_PRIO,
                "capi_cmn: Silence removal: initial %u, bytes removed per ch %lu. output %lu removed. out empty.",
                initial,
                valid_bytes_to_remove_per_channel,
                output[0]->buf_ptr->actual_data_len / out_mf_ptr->format.num_channels);

         output[0]->buf_ptr->actual_data_len = 0;
         *bytes_to_remove_per_channel_ptr    = valid_bytes_to_remove_per_channel - valid_bytes_per_channel;
         return CAPI_EOK;
      }
   }

   else if (out_mf_ptr->format.data_interleaving == CAPI_DEINTERLEAVED_UNPACKED)
   {
      uint32_t num_channels = 0;
      for (; num_channels < out_mf_ptr->format.num_channels; num_channels++)
      {
         valid_bytes_per_channel = output[0]->buf_ptr[num_channels].actual_data_len;

         if (valid_bytes_to_remove_per_channel > (valid_bytes_per_channel))
         {
            AR_MSG(DBG_MED_PRIO,
                   "capi_cmn: Silence removal: initial %u, bytes removed per ch %lu. output %lu removed. out empty.",
                   initial,
                   valid_bytes_to_remove_per_channel,
                   output[0]->buf_ptr->actual_data_len);

            // We drop the entire decoded frame, because the amount of data to be
            // removed is already greater than the data present in one decoded frame
            output[0]->buf_ptr->actual_data_len = 0;
         }
      }
      *bytes_to_remove_per_channel_ptr = valid_bytes_to_remove_per_channel - valid_bytes_per_channel;
      return CAPI_EOK;
   }

   if (out_mf_ptr->format.data_interleaving == CAPI_DEINTERLEAVED_PACKED)
   {
      addr_read_ptr  = output[0]->buf_ptr->data_ptr;
      addr_write_ptr = output[0]->buf_ptr->data_ptr;
      for (uint32_t num_channels = 0; num_channels < out_mf_ptr->format.num_channels; num_channels++)
      {
         if (initial)
         {
            addr_read_ptr += valid_bytes_to_remove_per_channel;
            memsmove(addr_write_ptr,
                     valid_bytes_per_channel - valid_bytes_to_remove_per_channel,
                     addr_read_ptr,
                     valid_bytes_per_channel - valid_bytes_to_remove_per_channel);
            addr_read_ptr += valid_bytes_per_channel - valid_bytes_to_remove_per_channel;
            // Since we are removing data from decoder output, we are incrementing addr_write_ptr by a smaller value
            // than original data length
            addr_write_ptr += valid_bytes_per_channel - valid_bytes_to_remove_per_channel;
         }
         else // trailing silence removal.
         {
            if (num_channels)
            {
               memsmove(addr_write_ptr,
                        valid_bytes_per_channel - valid_bytes_to_remove_per_channel,
                        addr_read_ptr,
                        valid_bytes_per_channel - valid_bytes_to_remove_per_channel);
            }
            addr_read_ptr += valid_bytes_per_channel;
            addr_write_ptr += valid_bytes_per_channel - valid_bytes_to_remove_per_channel;
         }
      }
      output[0]->buf_ptr->actual_data_len =
         valid_bytes_in_buffer - (valid_bytes_to_remove_per_channel * out_mf_ptr->format.num_channels);

      AR_MSG(DBG_MED_PRIO,
             "capi_cmn: Silence removal: initial %u, bytes removed per ch %lu, out size %lu",
             initial,
             valid_bytes_to_remove_per_channel,
             output[0]->buf_ptr->actual_data_len / out_mf_ptr->format.num_channels);

      // This is to return the amount of samples that are still not removed from starting data
      *bytes_to_remove_per_channel_ptr = *bytes_to_remove_per_channel_ptr - valid_bytes_to_remove_per_channel;
      return CAPI_EOK;
   }
   else if (out_mf_ptr->format.data_interleaving == CAPI_INTERLEAVED)
   {
      uint32_t total_valid_bytes     = valid_bytes_per_channel * out_mf_ptr->format.num_channels;
      uint32_t total_bytes_to_remove = valid_bytes_to_remove_per_channel * out_mf_ptr->format.num_channels;
      addr_read_ptr                  = output[0]->buf_ptr->data_ptr;
      addr_write_ptr                 = output[0]->buf_ptr->data_ptr;

      if (initial)
      {
         addr_read_ptr += total_bytes_to_remove;
         memsmove(addr_write_ptr,
                  total_valid_bytes - total_bytes_to_remove,
                  addr_read_ptr,
                  total_valid_bytes - total_bytes_to_remove);
      }
      else // trailing silence removal.
      {
         output[0]->buf_ptr->actual_data_len = total_valid_bytes - total_bytes_to_remove;
      }

      AR_MSG(DBG_MED_PRIO,
             "capi_cmn: Silence removal: initial %u, bytes removed per ch %lu, out size %lu",
             initial,
             valid_bytes_to_remove_per_channel,
             output[0]->buf_ptr->actual_data_len / out_mf_ptr->format.num_channels);

      *bytes_to_remove_per_channel_ptr = *bytes_to_remove_per_channel_ptr - valid_bytes_to_remove_per_channel;

      return CAPI_EOK;
   }
   else if (out_mf_ptr->format.data_interleaving == CAPI_DEINTERLEAVED_UNPACKED)
   {
      for (uint32_t num_channels = 0; num_channels < out_mf_ptr->format.num_channels; num_channels++)
      {
         addr_read_ptr  = output[0]->buf_ptr[num_channels].data_ptr;
         addr_write_ptr = output[0]->buf_ptr[num_channels].data_ptr;
         if (initial)
         {
            AR_MSG(DBG_HIGH_PRIO, "capi_cmn: Removing initial samples for gapless playback");
            addr_read_ptr += valid_bytes_to_remove_per_channel;

            memsmove(addr_write_ptr,
                     valid_bytes_per_channel - valid_bytes_to_remove_per_channel,
                     addr_read_ptr,
                     valid_bytes_per_channel - valid_bytes_to_remove_per_channel);
         }
         else
         {
            // Dont need to do memsmove here as addr_read_ptr and addr_write_ptr are same. We only need to update
            // actual data length.

            // memsmove(addr_write_ptr,
            //          valid_bytes_per_channel - valid_bytes_to_remove_per_channel,
            //          addr_read_ptr,
            //          valid_bytes_per_channel - valid_bytes_to_remove_per_channel);
         }

         output[0]->buf_ptr[num_channels].actual_data_len = valid_bytes_per_channel - valid_bytes_to_remove_per_channel;
         *bytes_to_remove_per_channel_ptr = *bytes_to_remove_per_channel_ptr - valid_bytes_to_remove_per_channel;

         AR_MSG(DBG_MED_PRIO,
                "capi_cmn: Silence removal: initial %u, bytes removed per ch %lu, out size %lu",
                initial,
                valid_bytes_to_remove_per_channel,
                output[0]->buf_ptr[num_channels].actual_data_len);
      }

      return CAPI_EOK;
   }
   else
   {
      AR_MSG(DBG_ERROR_PRIO, "Invalid data interleaving format. Returning without removing zeroes.");

      return CAPI_EFAILED;
   }
}

/* DM ext: This function raises an event to the fwk to inform the if module has enabled/disabled dm mode */
capi_err_t capi_cmn_raise_dm_disable_event(capi_event_callback_info_t *cb_info,
                                           uint32_t                    module_log_id,
                                           uint32_t                    disable)
{
   capi_err_t                        capi_result = CAPI_EOK;
   capi_event_data_to_dsp_service_t  event_payload;
   capi_event_info_t                 event_info;
   fwk_extn_dm_event_id_disable_dm_t cfg;
   cfg.disabled                          = disable;
   event_payload.param_id                = FWK_EXTN_DM_EVENT_ID_DISABLE_DM;
   event_payload.token                   = 0;
   event_payload.payload.data_ptr        = (int8_t *)&cfg;
   event_payload.payload.actual_data_len = sizeof(fwk_extn_dm_event_id_disable_dm_t);
   event_payload.payload.max_data_len    = sizeof(fwk_extn_dm_event_id_disable_dm_t);
   event_info.payload.data_ptr           = (int8_t *)&event_payload;
   event_info.payload.actual_data_len    = sizeof(event_payload);
   event_info.payload.max_data_len       = sizeof(event_payload);
   event_info.port_info.is_valid         = FALSE;
   capi_result = cb_info->event_cb(cb_info->event_context, CAPI_EVENT_DATA_TO_DSP_SERVICE, &event_info);
   if (CAPI_FAILED(capi_result))
   {
      AR_MSG(DBG_ERROR_PRIO,
             " Module 0x%lx failed to send report dm enable/disable event with result %d",
             module_log_id,
             capi_result);
   }
   return capi_result;
}

/* Populate Latest latched trigger ts into the payload */
capi_err_t capi_cmn_populate_trigger_ts_payload(capi_buf_t *params_ptr, stm_latest_trigger_ts_t *ts_struct_ptr)
{
   capi_err_t capi_result = CAPI_EOK;

   if (params_ptr->max_data_len < sizeof(capi_param_id_stm_latest_trigger_ts_ptr_t))
   {
      AR_MSG(DBG_ERROR_PRIO,
             "Invalid payload size %lu, needed size %lu",
             params_ptr->max_data_len,
             sizeof(capi_param_id_stm_latest_trigger_ts_ptr_t));
      return CAPI_EBADPARAM;
   }

   // populate current trigger timestamp in the return payload
   capi_param_id_stm_latest_trigger_ts_ptr_t *cfg_ptr =
      (capi_param_id_stm_latest_trigger_ts_ptr_t *)params_ptr->data_ptr;
   cfg_ptr->ts_ptr             = ts_struct_ptr;
   params_ptr->actual_data_len = sizeof(capi_param_id_stm_latest_trigger_ts_ptr_t);

   return capi_result;
}


void capi_cmn_dec_update_buffer_end_md(capi_stream_data_v2_t *in_stream_ptr,
                                       capi_stream_data_v2_t *out_stream_ptr,
                                       capi_err_t *           agg_process_result,
                                       bool_t *               error_recovery_done)
{
   // if result is not EOK or ENEEDMORE, or there is error_recovery_done, if we see end md we fill it and reset values
   // first level check will prevent unnecessary looping for end md
   if (((CAPI_EOK != *agg_process_result) && (CAPI_ENEEDMORE != *agg_process_result)) || (TRUE == *error_recovery_done))
   {
      module_cmn_md_list_t *node_ptr = in_stream_ptr->metadata_list_ptr;

      while (node_ptr)
      {
         module_cmn_md_t *md_ptr = node_ptr->obj_ptr;
         if (MODULE_CMN_MD_ID_BUFFER_END == md_ptr->metadata_id)
         {
            module_cmn_md_buffer_end_t *md_buf_end_ptr =
               (module_cmn_md_buffer_end_t *)((uint8_t *)md_ptr + sizeof(metadata_header_t));

            // error result
            capi_set_bits(&md_buf_end_ptr->flags,
                          MD_END_RESULT_FAILED,
                          MD_END_PAYLOAD_FLAGS_BIT_MASK_ERROR_RESULT,
                          MD_END_PAYLOAD_FLAGS_SHIFT_ERROR_RESULT);

            // result will be failed but if recovery is done we need to indicate to the clients
            if (TRUE == *error_recovery_done)
            {
               capi_set_bits(&md_buf_end_ptr->flags,
                             MD_END_RESULT_ERROR_RECOVERY_DONE,
                             MD_END_PAYLOAD_FLAGS_BIT_MASK_ERROR_RECOVERY_DONE,
                             MD_END_PAYLOAD_FLAGS_SHIFT_ERROR_RECOVERY_DONE);
            }

            // reset the flags
            *error_recovery_done = FALSE;
            *agg_process_result  = CAPI_EOK;

            AR_MSG(DBG_HIGH_PRIO,
                   "MD_DBG: Fill error in end md flags: node_ptr 0x%lx, md_ptr 0x%lx, md_id 0x%lx, buf idx msw:0x%lx "
                   "lsw:0x%lx, flag set %lx",
                   node_ptr,
                   md_ptr,
                   md_ptr->metadata_id,
                   md_buf_end_ptr->buffer_index_msw,
                   md_buf_end_ptr->buffer_index_lsw,
                   md_buf_end_ptr->flags);
            break;
         }
         node_ptr = node_ptr->next_ptr;
      }
   }
   return;
}

capi_err_t capi_cmn_dec_handle_metadata(capi_stream_data_v2_t *                in_stream_ptr,
                                        capi_stream_data_v2_t *                out_stream_ptr,
                                        intf_extn_param_id_metadata_handler_t *metadata_handler_ptr,
                                        module_cmn_md_list_t **                internal_md_list_pptr,
                                        uint32_t *                             in_len_before_process,
                                        capi_media_fmt_v2_t *                  out_media_fmt_ptr,
                                        uint32_t                               dec_algo_delay,
                                        capi_err_t                             process_result)
{
   capi_err_t result = CAPI_EOK;

   // handle eof propagation
   if (in_stream_ptr->flags.end_of_frame)
   {
      bool_t needmore = FALSE;
      // actual len here means what's consumed by CAPI.
      uint32_t pending_data = in_len_before_process[0] - in_stream_ptr->buf_ptr[0].actual_data_len;

      // if process returns needmore or
      // if no input remains
      // if no input is consumed and no output is produced
      if ((CAPI_ENEEDMORE == process_result) || (0 == pending_data) ||
          ((0 == in_stream_ptr->buf_ptr[0].actual_data_len) && (0 == out_stream_ptr->buf_ptr[0].actual_data_len)))
      {
         needmore = TRUE;
      }

      // pass EOF right away if it's not due to EOS, but otherwise, squeeze
      if (((0 == out_stream_ptr->buf_ptr[0].actual_data_len) && needmore) || (!in_stream_ptr->flags.marker_eos))
      {
         AR_MSG(DBG_LOW_PRIO, "Propagating EOF, eos%u", in_stream_ptr->flags.marker_eos);

         if ((pending_data) && (0 == out_stream_ptr->buf_ptr[0].actual_data_len))
         {
            AR_MSG(DBG_ERROR_PRIO,
                   "MD_DBG: process after: dropping input data %lu as no output got "
                   "produced for EOF",
                   pending_data);

            // no output produced => cannot squeeze anymore. hence drop any input.
            for (uint32_t b = 0; b < in_stream_ptr->bufs_num; b++)
            {
               in_stream_ptr->buf_ptr[b].actual_data_len = in_len_before_process[b];
            }
            pending_data = 0;
         }

         // if pending data is zero, move EOF (even if output got produced). If output got produced while there
         // was pending input data then we don't move EOF.
         if (0 == pending_data)
         {
            in_stream_ptr->flags.end_of_frame  = FALSE;
            out_stream_ptr->flags.end_of_frame = TRUE;
         }
      }
   }

   // propagate rest of the metadata
   intf_extn_md_propagation_t input_md_info;
   intf_extn_md_propagation_t output_md_info;
   memset(&input_md_info, 0, sizeof(input_md_info));
   input_md_info.df                          = CAPI_RAW_COMPRESSED;
   input_md_info.len_per_ch_in_bytes         = in_stream_ptr->buf_ptr[0].actual_data_len;
   input_md_info.initial_len_per_ch_in_bytes = in_len_before_process[0];

   memset(&output_md_info, 0, sizeof(output_md_info));
   output_md_info.df = CAPI_FIXED_POINT;
   output_md_info.len_per_ch_in_bytes =
      out_stream_ptr->buf_ptr[0].actual_data_len / out_media_fmt_ptr->format.num_channels;
   output_md_info.bits_per_sample = out_media_fmt_ptr->format.bits_per_sample;
   output_md_info.sample_rate     = out_media_fmt_ptr->format.sampling_rate;

   result |= metadata_handler_ptr->metadata_propagate(metadata_handler_ptr->context_ptr,
                                                      in_stream_ptr,
                                                      out_stream_ptr,
                                                      internal_md_list_pptr,
                                                      dec_algo_delay,
                                                      &input_md_info,
                                                      &output_md_info);

   if (CAPI_EOK != result)
   {
      AR_MSG(DBG_ERROR_PRIO, "MD_DBG: Decoder failed to propagate metadata with result 0x%x", result);
      return result;
   }

   return result;
}
